// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathGameInstance.h"
#include "UDEnemy.h"
#include "UDArena.h"
#include "UDPlayerCharacter.h"
#include "UDPlayerHUDWidget.h"
#include "UDRoundScreenWidget.h"
#include "Components/Button.h"
#include "Subsystems/WorldSubsystem.h"
#include "UDRoundRewardMenu.h"

#define LOCTEXT_NAMESPACE "PlayerHUD"

bool FEnemySpawnData::operator==(FEnemySpawnData const& Other)
{
	return EnemyClass == Other.EnemyClass;
}

void UUrsadeathGameInstance::Init()
{
	//Generate an array of the EnemySpawnData structures.
	TArray<FEnemySpawnData*> KnightSpawnData;
	KnightSpawnDataTable->GetAllRows("GameInstanceKnightSpawnDataMapInit", KnightSpawnData);

	//Populate the EnemyDataMap with the spawn data of each enemy.
	for (int i = 0; i < KnightSpawnData.Num(); i++)
	{
		FEnemySpawnData* SpawnDataEntry = KnightSpawnData[i];
		EnemyDataMap.Add(SpawnDataEntry->EnemyClass, SpawnDataEntry);
	}

	//Populate the enemy wave scheme array.
	WaveSchemeDataTable->GetAllRows("GameInstanceEnemyWaveSchemeArrayInit", EnemyWaveSchemes);

	SetupGame();

	//Generate the game's first round.
	GenerateRound(RoundNumber);
}

void UUrsadeathGameInstance::OnLevelSetup()
{
	UpdateRoundScreen();
}

void UUrsadeathGameInstance::PopulateRoundRewards()
{
	//Make sure the reward options are cleared out.
	KnightRewardOptions.Empty();

	//Set the options for the Knight Type reward.
	for (int i = 0; i < KnightRewardPool.Num() && i < 3; i++)
	{
		KnightRewardOptions.Add(KnightRewardPool[i]);
	}

	TArray<FRewardInfo> KnightRewardInfo;

	//Generate reward info for displaying the Knight Type Rewards
	for (int i = 0; i < KnightRewardOptions.Num(); i++)
	{
		FEnemySpawnData KnightTypeData = KnightRewardOptions[i];

		FRewardInfo KnightInfo;

		KnightInfo.RewardDescription = KnightTypeData.Description;
		KnightInfo.RewardImage = KnightTypeData.EnemyIcon;

		KnightRewardInfo.Add(KnightInfo);
	}

	//Display the Knight Type Rewards to the player's UI.
	PlayerCharacter->GetRoundScreenWidget()->GetKnightRewardMenu()->SetRewardOptions(KnightRewardInfo);
}

void UUrsadeathGameInstance::StartWave(FEnemyWave Wave)
{
	//Show the wave that's about to spawn to the player
	UUDPlayerHUDWidget* PlayerHUDWidget = PlayerCharacter->GetHUDWidget();
	PlayerHUDWidget->DisplayEnemyWave(Wave);
	PlayerHUDWidget->DisplayAnnouncement(FText::Format(LOCTEXT("EnemyWaveNumberAnnouncement", "Wave {0}"), RoundWaveNumber+1), WaveStartDelay);

	//Set the wave to spawn after WaveStartDelay seconds passes.
	FTimerHandle WaveStartTimerHandle;
	FTimerDelegate WaveStartDelegate = FTimerDelegate::CreateUObject(this, &UUrsadeathGameInstance::StartWaveInstant, Wave);
	GetWorld()->GetTimerManager().SetTimer(WaveStartTimerHandle, WaveStartDelegate, WaveStartDelay, false);
}

void UUrsadeathGameInstance::StartWaveInstant(FEnemyWave Wave)
{
	PlayerCharacter->GetHUDWidget()->DisplayAnnouncement(LOCTEXT("EnemyWaveStartAnnouncement", "Begin!"), 2);

	GameArena->SpawnEnemyWave(Wave);
}

void UUrsadeathGameInstance::GenerateRound(int RoundIndex)
{
	//Get the number of waves that should be generated for this round.
	int RoundWaveCount = RoundWaveCounts[RoundIndex];

	//Empty out the RoundWaves array, reserving space for the round's number of waves.
	CurrentRoundWaves.Empty(RoundWaveCount);

	//Generate the waves that will be in the round.
	for (int i = 0; i < RoundWaveCount; i++)
	{
		//Get the wave scheme we'll be using.
		FEnemyWaveScheme WaveScheme = *EnemyWaveSchemes[AbsoluteWaveNumber + i];
		FEnemyWave EnemyWave = GenerateEnemyWave(WaveScheme);

		//Add the wave that was just generated to the round.
		CurrentRoundWaves.Add(EnemyWave);
	}
}

FEnemyWave UUrsadeathGameInstance::GenerateEnemyWave(FEnemyWaveScheme WaveScheme)
{
	FEnemyWave EnemyWave;

	//Copy the common data from the WaveScheme to the wave.
	EnemyWave.WaveData = WaveScheme.WaveCommonData;

	//Make a copy of the Knight Pool that can be editted.
	TArray<TSubclassOf<AUDEnemy>> KnightPool = KnightSpawnPool;

	//An array of the Knight Types that will be in the wave we are generating. This is used to help us set the actual amount of a Knight Type that will be in the wave.
	TArray<TSubclassOf<AUDEnemy>> WaveKnightTypes;

	//If UseNewKnight is set to true, forcibly add the UnchosenKnight to the wave. 
	if (WaveScheme.bUseNewKnight)
	{
		EnemyWave.KnightCounts.Add(UnchosenKnightSpawnData.EnemyClass, 1);

		//Record that the Knight Type is in the wave for later use.
		WaveKnightTypes.Add(UnchosenKnightSpawnData.EnemyClass);

		KnightPool.Remove(UnchosenKnightSpawnData.EnemyClass);
	}

	//Add a random Knight Type to the wave's KnightCount map until the map as as many entries as the Wave Scheme's Knight Types. Also populate WaveKnightTypes for later use.
	for (int i = 0; EnemyWave.KnightCounts.Num() < WaveScheme.KnightTypes; i++)
	{
		//Pick a Knight from a random index in the Knight Pool. The index is remembered to make removing the Knight a bit more efficient later.
		int KnightTypeIndex = RandomStream.RandRange(0, KnightPool.Num()-1);
		TSubclassOf<AUDEnemy> KnightType = KnightPool[KnightTypeIndex];

		//Add the Knight Type to the Knight Count map. The type's count always starts at 1 so that their is always at least one Knight of each added type.
		EnemyWave.KnightCounts.Add(KnightType,1);

		//Record that the Knight Type is in the wave for later use.
		WaveKnightTypes.Add(KnightType);

		//Remove the Knight type from the spawn pool.
		KnightPool.RemoveAt(KnightTypeIndex);
	}

	//Set randomly distribute the wave scheme's Knight Count between all of the wave's Knight Types. This process is heavily simplified if there is only 1 type of knight in the wave. 
	if (WaveScheme.KnightTypes == 1)
	{
		//If there is only one Knight Type, we can simply set it to have the Knight Count.
		EnemyWave.KnightCounts[WaveKnightTypes[0]] = WaveScheme.KnightCount;
	}
	else
	{
		//Since we start each KnightCount at 1 when they are first added to the wave, we need to subtract 1 from the Knight Count for each Knight Type.
		int ModifiedKnightCount = WaveScheme.KnightCount - WaveScheme.KnightTypes;

		//Randomize the amount of Knights for each Knight type. I couldn't find an algorithm for distributing an integer randomly, so we're just incrementing a random Knight's count by 1 each iteration!
		for (int i = 0; i < ModifiedKnightCount; i++)
		{
			//Get a random knight type that is in the wave.
			TSubclassOf<AUDEnemy> KnightType = WaveKnightTypes[RandomStream.RandRange(0, WaveKnightTypes.Num() - 1)];

			//Increment the Knight Type's count.
			EnemyWave.KnightCounts[KnightType]++;
		}
	}

	return EnemyWave;
}

void UUrsadeathGameInstance::SetUnchosenKnight(TSubclassOf<AUDEnemy> NewKnightType)
{
	for (int i = 0; i < CurrentRoundWaves.Num(); i++)
	{
		//Get a pointer to the wave being modified this iteration.
		FEnemyWave* Wave = &CurrentRoundWaves[i];
		
		//Make sure wave contains an unchosen knight.
		if (Wave->KnightCounts.Contains(UnchosenKnightSpawnData.EnemyClass))
		{
			//Set what the enemy count for our new enemy type will be from the count of the UnchosenKnight. Then, remove the UnchosenKnight from the listing of enemy counts.
			int32 EnemyCount;
			Wave->KnightCounts.RemoveAndCopyValue(UnchosenKnightSpawnData.EnemyClass, EnemyCount);

			//Add the new enemy type to the wave.
			Wave->KnightCounts.Add(NewKnightType, EnemyCount);
		}	
	}

	//Update the round screen so that it shows the new enemy.
	UpdateRoundScreen();
}

const FEnemySpawnData UUrsadeathGameInstance::GetSpawnDataEntry(TSubclassOf<AUDEnemy> EnemyClass)
{
	if (EnemyClass == UnchosenKnightSpawnData.EnemyClass)
	{
		return UnchosenKnightSpawnData;
	}
	else
	{
		return *EnemyDataMap[EnemyClass];
	}	
}

void UUrsadeathGameInstance::StartRound()
{
	//Get the next wave.
	//(Look, we all know that the index should always be 0, but the game might not know that. Better to have the game freak out here than give a confusing error 1 wave later.)
	FEnemyWave NextWave = CurrentRoundWaves[RoundWaveNumber];

	StartWave(NextWave);
}

void UUrsadeathGameInstance::ProcessEndWave()
{
	AbsoluteWaveNumber++;

	//Continue the wave if there are still waves left. Otherwise, generate the next round.
	if (CurrentRoundWaves.IsValidIndex(RoundWaveNumber + 1))
	{
		RoundWaveNumber++;

		FEnemyWave NextWave = CurrentRoundWaves[RoundWaveNumber];

		//Begin the start sequence for the next wave.
		StartWave(NextWave);
	}
	else
	{
		RoundWaveNumber = 0;

		PopulateRoundRewards();

		//Generate the game's next round, if one exists. The game will wait for StartRound() to be called before spawning anything.
		if(RoundWaveCounts.IsValidIndex(RoundNumber + 1))
		{
			RoundNumber++;
			GenerateRound(RoundNumber);

			//If there are no KnightRewards to cho
			if (KnightRewardOptions.Num() == 0)
			{
				PlayerCharacter->GetRoundScreenWidget()->EnableRoundStart();
			}

			UpdateRoundScreen();

			//Announce to the player that the round has been completed.
			PlayerCharacter->GetHUDWidget()->DisplayAnnouncement(LOCTEXT("EnemyWaveStartAnnouncement", "Round Complete!"), 2);
		}
		else
		{
			//If there are no more rounds and the player has completed the demo, congratulate them!
			PlayerCharacter->GetHUDWidget()->DisplayAnnouncement(LOCTEXT("GameCompleteAnnouncement", "That's all Folks! Thanks for playing!"), 0);
		}

		//Restore the player to full health when they complete a round.
		PlayerCharacter->RestoreHealth(999);
	}
}

void UUrsadeathGameInstance::ResetGame()
{
	RoundNumber = 0;

	RoundWaveNumber = 0;
	AbsoluteWaveNumber = 0;

	//Reset the Knight spawn pool
	KnightSpawnPool.Empty(1);

	SetupGame();

	GenerateRound(RoundNumber);
}

FEnemySpawnData UUrsadeathGameInstance::GetSquireSpawnData()
{
	return SquireSpawnData;
}

void UUrsadeathGameInstance::UpdateRoundScreen()
{
	//Update the player UI. We display RoundNumber + 1 so the game doesn't show the player nerdy programmer numbers.
	PlayerCharacter->GetRoundScreenWidget()->DisplayRound(RoundNumber + 1, CurrentRoundWaves);
}

int UUrsadeathGameInstance::GetCurrentRound()
{
	return RoundNumber;
}

int UUrsadeathGameInstance::GetAbsoluteWave()
{
	return AbsoluteWaveNumber;
}

template<typename T>
void UUrsadeathGameInstance::ShuffleArray(TArray<T> &Array)
{
	int NumShuffles = Array.Num() - 1;
	for (int i = 0; i < NumShuffles; i++)
	{
		int SwapIndex = RandomStream.RandRange(i, NumShuffles);
		Array.Swap(i, SwapIndex);
	}
}

void UUrsadeathGameInstance::AddKnightReward()
{
	UUDRoundRewardMenu* KnightRewardMenu = PlayerCharacter->GetRoundScreenWidget()->GetKnightRewardMenu();
	int32 KnightRewardIndex = KnightRewardMenu->GetRewardSelected();

	//Get the knight reward spawn data
	FEnemySpawnData RewardData = KnightRewardOptions[KnightRewardIndex];

	//Get the knight type the player chose from their round rewards.
	TSubclassOf<AUDEnemy> KnightRewardClass = RewardData.EnemyClass;

	//Add the new knight to the spawn pool.
	KnightSpawnPool.Add(KnightRewardClass);

	//Set the "Unchosen" Knight instances in our round to the new knight type.
	SetUnchosenKnight(KnightRewardClass);

	//Remove the chosen Knight from the Knight reward pool.
	KnightRewardPool.Remove(RewardData);

	//If there are no more knight rewards to be given, remove the unchosen knight from the spawn pool.
	if (KnightRewardPool.Num() == 0)
	{
		KnightSpawnPool.Remove(UnchosenKnightSpawnData.EnemyClass);
	}
}

void UUrsadeathGameInstance::SetupGame()
{
	//Initialize the game's random seed.
	RandomStream.Initialize(GameSeedName);

	//Get the spawn data from the data table.
	TArray<FEnemySpawnData*> KnightSpawnData;
	KnightSpawnDataTable->GetAllRows("GameInstanceKnightSpawnDataMapInit", KnightSpawnData);

	//Populate the KnightReward pool with the spawn data of each enemy.
	KnightRewardPool.Empty(KnightSpawnData.Num());
	for (int i = 0; i < KnightSpawnData.Num(); i++)
	{
		FEnemySpawnData* SpawnDataEntry = KnightSpawnData[i];

		KnightRewardPool.Add(*SpawnDataEntry);
	}

	//Shuffle the Knight rewards.
	ShuffleArray<FEnemySpawnData>(KnightRewardPool);

	//Add the UnchosenKnight to the spawn pool.
	KnightSpawnPool.Add(UnchosenKnightSpawnData.EnemyClass);
}

void UUrsadeathGameInstance::FinalizePlayerSetup(AUDPlayerCharacter* Player)
{
	PlayerCharacter = Player;

	UpdateRoundScreen();

	PopulateRoundRewards();
}

#undef LOCTEXT_NAMESPACE



