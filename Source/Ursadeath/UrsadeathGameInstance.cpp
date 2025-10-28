// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathGameInstance.h"
#include "UDEnemy.h"
#include "UDArena.h"
#include "UDPlayerCharacter.h"
#include "UDPlayerHUDWidget.h"
#include "UDRoundScreenWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Subsystems/WorldSubsystem.h"
#include "UDRoundRewardMenu.h"
#include "UDPlayerUpgrade.h"
#include "UDPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UDEnemyUpgrade.h"

#define LOCTEXT_NAMESPACE "PlayerHUD"

bool FEnemySpawnData::operator==(FEnemySpawnData const& Other) const
{
	return EnemyClass == Other.EnemyClass;
}

bool FEnemySpawnEntry::operator==(FEnemySpawnEntry const& Other) const
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

	//Get an array of the enemy upgrades
	TArray<FEnemyUpgradeData*> EnemyUpgradeData;
	EnemyUpgradeDataTable->GetAllRows("GameInstanceEnemyUpgradeInit", EnemyUpgradeData);

	for (int i = 0; i < EnemyUpgradeData.Num(); i++)
	{
		FEnemyUpgradeData UpgradeDataEntry = *EnemyUpgradeData[i];

		/** If the enemy upgrade is enabled, create an instance of it and */
		if (UpgradeDataEntry.bEnabled)
		{
			UUDEnemyUpgrade* UpgradeInstance = UUDEnemyUpgrade::CreateUpgrade(UpgradeDataEntry.UpgradeClass, this);

			FEnemyUpgradeReward EnemyUpgradeReward;
			EnemyUpgradeReward.UpgradeData = UpgradeDataEntry;
			EnemyUpgradeReward.UpgradeInstance = UpgradeInstance;

			EnemyUpgradeInstances.Add(EnemyUpgradeReward);
			
			EnemyUpgradeReferences.Add(UpgradeInstance);
		}
	}

	//Populate the enemy wave scheme array.
	WaveSchemeDataTable->GetAllRows("GameInstanceEnemyWaveSchemeArrayInit", EnemyWaveSchemes);

		//Iterate through the waves to get the number of waves per round.
	//Tracks which round we are counting the waves for.
	int RoundBeingCounted = 0;
	//We start with the first wave already counted and skipped over since that will always be the start of a new round (and therefore mess up the loop!).
	RoundWaveCounts.Add(1);
	for (int i = 1; i < EnemyWaveSchemes.Num(); i++)
	{
		FEnemyWaveScheme* WaveScheme = EnemyWaveSchemes[i];

		//If the wave represents the start of a new round, increment the round being counted to and add a new index to the RoundWaveCounts array. Otherwise, simply increment the round wave count value.
		if (WaveScheme->NewRoundType != ENewRoundType::NONE)
		{
			RoundBeingCounted++;
			RoundWaveCounts.Add(1);
		}
		else
		{
			RoundWaveCounts[RoundBeingCounted]++;
		}
	}

	SetupGame();

	//Generate the game's first round.
	GenerateRound(RoundNumber, StartingWave);
}

void UUrsadeathGameInstance::RemoveFromKnightPool(TSubclassOf<AUDEnemy> EnemyClass)
{
	FEnemySpawnEntry RemovedEntry;
	RemovedEntry.EnemyClass = EnemyClass;

	KnightSpawnPool.Remove(RemovedEntry);
}

void UUrsadeathGameInstance::PopulateKnightRewards()
{
	//Make sure the reward options are cleared out.
	KnightRewardOptions.Empty();

	//We populate this array as we select the rewards. We then feed this info to the UI for displaying.
	TArray<FRewardInfo> KnightRewardOptionInfo;

	//Set the options for the Knight Type reward.
	for (int i = 0; i < KnightRewardPool.Num() && i < KnightRewardMenu->GetMaxRewardOptions(); i++)
	{
		FEnemySpawnData KnightRewardData = KnightRewardPool[i];

		KnightRewardOptions.Add(KnightRewardData);

		FRewardInfo KnightInfo;

		KnightInfo.RewardDescription = KnightRewardData.Description;
		KnightInfo.RewardImage = KnightRewardData.EnemyIcon;

		KnightRewardOptionInfo.Add(KnightInfo);
	}

	//Display the Knight Type Rewards to the player's UI.
	KnightRewardMenu->SetRewardOptions(KnightRewardOptionInfo);
}

void UUrsadeathGameInstance::PopulateEnemyUpgradeRewards()
{
	EnemyUpgradeRewardOptions.Empty();

	//Shuffle the enemy upgrade reward pool. We shuffle before each time the pool is populated because the addition of new Knight types also causes more enemy upgrades to be added!
	ShuffleArray(EnemyUpgradeRewardPool);

	//We populate this array as we select the rewards. We then feed this info to the UI for displaying.
	TArray<FRewardInfo> UpgradeRewardOptionInfo;

	for (int i = 0; i < EnemyUpgradeRewardPool.Num() && i < EnemyUpgradeRewardMenu->GetMaxRewardOptions(); i++)
	{
		FEnemyUpgradeReward UpgradeReward = EnemyUpgradeRewardPool[i];

		EnemyUpgradeRewardOptions.Add(UpgradeReward);

		FRewardInfo UpgradeInfo;

		UpgradeInfo.RewardDescription = UpgradeReward.UpgradeData.Description;;
		UpgradeInfo.RewardImage = UpgradeReward.UpgradeData.UIImage;

		UpgradeRewardOptionInfo.Add(UpgradeInfo);
	}

	EnemyUpgradeRewardMenu->SetRewardOptions(UpgradeRewardOptionInfo);
}

void UUrsadeathGameInstance::PopulateUpgradeRewards()
{
	UpgradeRewardOptions.Empty();

	//We populate this array as we select the rewards. We then feed this info to the UI for displaying.
	TArray<FRewardInfo> UpgradeRewardOptionInfo;

	//Take upgrades from the beginning of the UpgradeRewardPool until either the pool is exhausted or the UpgradeMenu's max rewards is reached.
	while (UpgradeRewardPool.Num() > 0 && UpgradeRewardOptions.Num() < UpgradeRewardMenu->GetMaxRewardOptions())
	{
		FPlayerUpgradeData* UpgradeReward = UpgradeRewardPool[0];
		UpgradeRewardOptions.Add(UpgradeReward);

		//Create and set the struct for displaying the upgrade's icon, name, and description.
		FRewardInfo UpgradeInfo;
		UpgradeInfo.RewardDescription = UpgradeReward->Description;
		UpgradeInfo.RewardImage = UpgradeReward->UIIcon;

		UpgradeRewardOptionInfo.Add(UpgradeInfo);

		UpgradeRewardPool.Remove(UpgradeReward);
	}

	UpgradeRewardMenu->SetRewardOptions(UpgradeRewardOptionInfo);
}

template<class T>
void UUrsadeathGameInstance::DiscardIntoPool(TArray<T>& PoolArray, TArray<T>& SourceArray)
{
	while (SourceArray.Num() > 0)
	{
		//Get a random index from the source array.
		int index = RandomStream.RandRange(0, SourceArray.Num() - 1);
		//The get element at the randomly chosen index.
		auto element = SourceArray[index];

		//Append the element at the end of the pool array (it is assumed that the element was removed from the pool before being added to the source).
		PoolArray.Add(element);

		//Remove the element from the source array.
		SourceArray.Remove(element);
	}
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

		//Other Side FX
	
	//Respawn a single one of the Health Pickups
	GameArena->ReactivateSingleHealthPickup();
}

void UUrsadeathGameInstance::StartWaveInstant(FEnemyWave Wave)
{
	//Clear corpses at the start of each wave.
	GameArena->ClearCorpses();

	PlayerCharacter->GetHUDWidget()->DisplayAnnouncement(LOCTEXT("EnemyWaveStartAnnouncement", "Begin!"), 2);

	GameArena->SpawnEnemyWave(Wave);
}

void UUrsadeathGameInstance::GenerateRound(int RoundIndex, int WavesSkipped)
{
	//Get the number of waves that should be generated for this round.
	int RoundWaveCount = RoundWaveCounts[RoundIndex];

	//Empty out the RoundWaves array, reserving space for the round's number of waves.
	CurrentRoundWaves.Empty(RoundWaveCount);

	//We still generate the entire round, even if we skip a wave. This value slides back the AbsoluteWaveNumber back to the start of the Round.
	int FirstWaveIndex = AbsoluteWaveNumber - WavesSkipped;

	//If the new round has a new enemy type, add the Unchosen Knight to the pool of enemies.
	if (WavesSkipped == 0)
	{
		if (EnemyWaveSchemes[FirstWaveIndex]->NewRoundType == ENewRoundType::NEW_ENEMY)
		{
			FEnemySpawnEntry UnchosenSpawnEntry;
			UnchosenSpawnEntry.EnemyClass = UnchosenKnightSpawnData[UnchosenEnemies].EnemyClass;
			KnightSpawnPool.Add(UnchosenSpawnEntry);
			UnchosenEnemies++;

			NewRoundProgessions.Add(ENewRoundType::NEW_ENEMY);
		}
		else if (EnemyWaveSchemes[FirstWaveIndex]->NewRoundType == ENewRoundType::UPGRADE_ENEMY)
		{
			NewRoundProgessions.Add(ENewRoundType::UPGRADE_ENEMY);
		}
	}

	//Generate the waves that will be in the round.
	for (int i = 0; i < RoundWaveCount; i++)
	{
		//Get the wave scheme we'll be using.
		FEnemyWaveScheme WaveScheme = *EnemyWaveSchemes[FirstWaveIndex + i];

		FEnemyWave EnemyWave = GenerateEnemyWave(WaveScheme);

		//Add the wave that was just generated to the round.
		CurrentRoundWaves.Add(EnemyWave);
	}
}

void UUrsadeathGameInstance::GenerateRound(int RoundIndex)
{
	GenerateRound(RoundIndex, 0);
}

FEnemyWave UUrsadeathGameInstance::GenerateEnemyWave(FEnemyWaveScheme WaveScheme)
{
	FEnemyWave EnemyWave;

	//Copy the common data from the WaveScheme to the wave.
	EnemyWave.WaveData = WaveScheme.WaveCommonData;

	//Make a copy of the Knight Pool that can be editted.
	TArray<FEnemySpawnEntry> KnightPool = KnightSpawnPool;

	//An array of the Knight Types that will be in the wave we are generating. This is used to help us set the actual amount of a Knight Type that will be in the wave.
	TArray<TSubclassOf<AUDEnemy>> WaveKnightTypes;

	//If UseNewKnight is set to true, forcibly add the UnchosenKnight to the wave. 
	if (WaveScheme.bUseNewKnight)
	{
		FEnemySpawnParams SpawnParams;
		SpawnParams.Count = 1;
		EnemyWave.KnightParams.Add(UnchosenKnightSpawnData[0].EnemyClass, SpawnParams);

		//Record that the Knight Type is in the wave for later use.
		WaveKnightTypes.Add(UnchosenKnightSpawnData[0].EnemyClass);

		FEnemySpawnEntry UnchosenSpawnEntry;
		KnightPool.Remove(UnchosenSpawnEntry);
	}

	//Add a random Knight Type to the wave's KnightCount map until the map as as many entries as the Wave Scheme's Knight Types. Also populate WaveKnightTypes for later use.
	for (int i = 0; EnemyWave.KnightParams.Num() < WaveScheme.KnightTypes; i++)
	{
		//Pick a Knight from a random index in the Knight Pool. The index is remembered to make removing the Knight a bit more efficient later.
		int KnightTypeIndex = RandomStream.RandRange(0, KnightPool.Num()-1);
		FEnemySpawnEntry SpawnEntry = KnightPool[KnightTypeIndex];

		TSubclassOf<AUDEnemy> KnightClass = SpawnEntry.EnemyClass;
		UUDEnemyUpgrade* KnightUpgrade = SpawnEntry.Upgrade;

		//Add the Knight Type to the Knight Count map. The type's count always starts at 1 so that their is always at least one Knight of each added type.
		FEnemySpawnParams SpawnParams;
		SpawnParams.Count = 1;
		SpawnParams.Upgrade = KnightUpgrade;
		EnemyWave.KnightParams.Add(KnightClass, SpawnParams);

		//Record that the Knight Type is in the wave for later use.
		WaveKnightTypes.Add(KnightClass);

		//Remove the Knight type from the spawn pool.
		KnightPool.RemoveAt(KnightTypeIndex);
	}

	//Set randomly distribute the wave scheme's Knight Count between all of the wave's Knight Types. This process is heavily simplified if there is only 1 type of knight in the wave. 
	if (WaveScheme.KnightTypes == 1)
	{
		//If there is only one Knight Type, we can simply set it to have the Knight Count.
		EnemyWave.KnightParams[WaveKnightTypes[0]].Count = WaveScheme.KnightCount;
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
			EnemyWave.KnightParams[KnightType].Count++;
		}
	}

	return EnemyWave;
}

void UUrsadeathGameInstance::SetUnchosenKnight(TSubclassOf<AUDEnemy> NewKnightType, int32 UnchosenKnightIndex)
{
	for (int i = 0; i < CurrentRoundWaves.Num(); i++)
	{
		//Get a pointer to the wave being modified this iteration.
		FEnemyWave* Wave = &CurrentRoundWaves[i];
		
		//Make sure wave contains an unchosen knight.
		if (Wave->KnightParams.Contains(UnchosenKnightSpawnData[UnchosenKnightIndex].EnemyClass))
		{
			//Set what the enemy count for our new enemy type will be from the count of the UnchosenKnight. Then, remove the UnchosenKnight from the listing of enemy counts.
			FEnemySpawnParams SpawnParams;
			Wave->KnightParams.RemoveAndCopyValue(UnchosenKnightSpawnData[UnchosenKnightIndex].EnemyClass, SpawnParams);

			//Add the new enemy type to the wave.
			Wave->KnightParams.Add(NewKnightType, SpawnParams);
		}	
	}

	//Remove the now-chosen unchosen knight from the spawn pool.
	RemoveFromKnightPool(UnchosenKnightSpawnData[UnchosenKnightIndex].EnemyClass);

	//Decrement the unchosen enemies now that one of them has been chosen.
	UnchosenEnemies--;

	//Update the round screen so that it shows the new enemy.
	UpdateRoundScreen();
}

const FEnemySpawnData UUrsadeathGameInstance::GetSpawnDataEntry(TSubclassOf<AUDEnemy> EnemyClass)
{
	if (EnemyDataMap.Contains(EnemyClass))
	{
		return *EnemyDataMap[EnemyClass];
	}
	else
	{
		for (int i = 0; i < UnchosenKnightSpawnData.Num(); i++)
		{
			if (UnchosenKnightSpawnData[i].EnemyClass == EnemyClass)
			{
				return UnchosenKnightSpawnData[i];
			}
		}

		//The script is never meant to actually get here; If it does, the means there are not enough UnchosenKnight entries in UnchosenKnightSpawnData.
		return UnchosenKnightSpawnData[-1];
	}
}

void UUrsadeathGameInstance::StartRound()
{
	//Get the next wave.
	//(Look, we all know that the index should always be 0, but the game might not know that. Better to have the game freak out here than give a confusing error 1 wave later.)
	FEnemyWave NextWave = CurrentRoundWaves[RoundWaveNumber];

	//Respawn the health pickups in the arena. We respawn them both at the end and beginning of rounds incase the player decided to be goofy and hit one for no reason.
	GameArena->ReactivateHealthPickups();
	//Make the health pickups no longer respawn after collecting them. Instead, a single one will respawn when the wave ends.
	GameArena->SetHealthPickupCooldownMode(false);

	//Start the rounds first wave.
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

		//Discard the unchosen reward options back into their pools.
		//DiscardIntoPool(KnightRewardPool, KnightRewardOptions);
		DiscardIntoPool(UpgradeRewardPool, UpgradeRewardOptions);

		//Generate the game's next round, if one exists. The game will wait for StartRound() to be called before spawning anything.
		if(RoundWaveCounts.IsValidIndex(RoundNumber + 1))
		{
			RoundNumber++;
			GenerateRound(RoundNumber);

			//If there are no KnightRewards to choose from, enable the Round Start Button (Normally the button is disabled until the player selects a new Knight for that round). Otherwise, set the start button text to tell the player to select an enemy type.
			if (KnightRewardOptions.Num() == 0)
			{
				PlayerCharacter->GetRoundScreenWidget()->TryEnableRoundStart();
			}
			else
			{
				PlayerCharacter->GetRoundScreenWidget()->GetStartButtonText()->SetText(LOCTEXT("StartButtonChooseEnemy", "You must choose a foe!"));
			}

			//Populate the rewards.
			if (NewRoundProgessions[0] == ENewRoundType::NEW_ENEMY)
			{
				PopulateKnightRewards();

				KnightRewardMenu->SetVisibility(ESlateVisibility::Visible);
				EnemyUpgradeRewardMenu->SetVisibility(ESlateVisibility::Collapsed);

				//Players only get an upgrade on rounds where a new knight type is being chosen.
				PopulateUpgradeRewards();
			}
			else
			{
				PopulateEnemyUpgradeRewards();

				KnightRewardMenu->SetVisibility(ESlateVisibility::Collapsed);
				EnemyUpgradeRewardMenu->SetVisibility(ESlateVisibility::Visible);

				//Tell the player that they don't get an upgrade!
				UpgradeRewardMenu->ShowLockedMessage();
			}

			UpdateRoundScreen();

			//Announce to the player that the round has been completed.
			PlayerCharacter->GetHUDWidget()->DisplayAnnouncement(LOCTEXT("EnemyWaveStartAnnouncement", "Round Complete!"), 2);
		}
		else
		{
			//If there are no more rounds and the player has completed the demo, congratulate them!
			PlayerCharacter->GetHUDWidget()->DisplayAnnouncement(LOCTEXT("GameCompleteAnnouncement", "That's all Folks! Thanks for playing!"), 0);

			PlayerCharacter->GetRoundScreenWidget()->GetStartButtonText()->SetText(LOCTEXT("StartButtonGameComplete", "They are all defeated!"));
		}

		//Restore the player to full health when they complete a round.
		PlayerCharacter->RestoreHealth(999);

		//Respawn the health pickups in the arena. We respawn them both at the end and beginning of rounds incase the player decided to be goofy and hit one for no reason.
		GameArena->ReactivateHealthPickups();

		//Set all of the health pickups to respawn after being collected incase the player wants to mess around with hitting them.
		GameArena->SetHealthPickupCooldownMode(true);
	}
}

void UUrsadeathGameInstance::ResetGame()
{
	RoundNumber = 0;

	RoundWaveNumber = 0;
	AbsoluteWaveNumber = 0;

	//Empty the Round Progressions for the player.
	NewRoundProgessions.Empty();
	UnchosenEnemies = 0;

	//Reset the Knight spawn pool.
	KnightSpawnPool.Empty();

	//Reset the enemy upgrade pool
	EnemyUpgradeRewardPool.Empty();

	//Reset the Upgrade pool
	UpgradeRewardPool.Empty();

	SetupGame();

	//Generate the game's first round.
	GenerateRound(RoundNumber, StartingWave);

	//Make sure the player is not in the UI input mode (most likely they are if the game is being reset.
	PlayerCharacter->GetUDPlayerController()->SetInputMode(FInputModeGameOnly());

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GameLevel);
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

template<class T>
void UUrsadeathGameInstance::ShuffleArray(TArray<T> &Array)
{
	//Iterate through each index, swapping its element with that of another random index. I.e. A Fisher-Yates shuffle.
	int NumSwaps = Array.Num() - 1;
	for (int i = 0; i < NumSwaps; i++)
	{
		int SwapIndex = RandomStream.RandRange(i, NumSwaps);
		Array.Swap(i, SwapIndex);
	}
}

void UUrsadeathGameInstance::CheckEnemyProgression()
{
	NewRoundProgessions.RemoveAt(0);

	if (NewRoundProgessions.Num() > 0)
	{
		ENewRoundType NextProgression = NewRoundProgessions[0];

		if (NextProgression == ENewRoundType::NEW_ENEMY)
		{
			PopulateKnightRewards();

			KnightRewardMenu->SetVisibility(ESlateVisibility::Visible);
			EnemyUpgradeRewardMenu->SetVisibility(ESlateVisibility::Collapsed);
		}
		else //We assume that the new round type is UPGRADE_ENEMY if it is not NEW_ENEMY since "NONE" should never actually be in the progression.
		{
			PopulateEnemyUpgradeRewards();

			KnightRewardMenu->SetVisibility(ESlateVisibility::Collapsed);
			EnemyUpgradeRewardMenu->SetVisibility(ESlateVisibility::Visible);
		}

		//If this is the last reward, then tell the menus not to let the player select something else.
		if (NewRoundProgessions.Num() == 1)
		{
			KnightRewardMenu->SetRepeatReward(false);
			EnemyUpgradeRewardMenu->SetRepeatReward(false);
		}
	}
}

void UUrsadeathGameInstance::AddKnightReward()
{
	int32 KnightRewardIndex = KnightRewardMenu->GetRewardSelected();

	//Get the knight reward spawn data
	FEnemySpawnData RewardData = KnightRewardOptions[KnightRewardIndex];

	//Get the knight type the player chose from their round rewards.
	TSubclassOf<AUDEnemy> KnightRewardClass = RewardData.EnemyClass;

	//Add the new knight to the spawn pool.
	FEnemySpawnEntry SpawnEntry;
	SpawnEntry.EnemyClass = KnightRewardClass;
	KnightSpawnPool.Add(SpawnEntry);

	//Set the "Unchosen" Knight instances in our round to the new knight type.
	SetUnchosenKnight(KnightRewardClass, UnchosenEnemies-1);

	//Remove the chosen Knight from the Knight reward pool.
	KnightRewardPool.Remove(RewardData);

	//Add an Enemy Upgrade Reward containing the chosen knight for each upgrade in the game.
	for (int i = 0; i < EnemyUpgradeInstances.Num(); i++)
	{
		FEnemyUpgradeReward UpgradeReward = EnemyUpgradeInstances[i];

		UpgradeReward.EnemyUpgraded = KnightRewardClass;

		//Change the upgrade's title to include the name of the enemy type being upgraded.
		UpgradeReward.UpgradeData.Description.Title = FText::Format(LOCTEXT("EnemyUpgradeInit", "{UpgradeTitle} {EnemyTitle}"), UpgradeReward.UpgradeData.Description.Title, EnemyDataMap[UpgradeReward.EnemyUpgraded]->Description.Title);

		EnemyUpgradeRewardPool.Add(UpgradeReward);
	}

	CheckEnemyProgression();
}

void UUrsadeathGameInstance::AddEnemyUpgradeReward()
{
	int32 RewardIndex = EnemyUpgradeRewardMenu->GetRewardSelected();

	FEnemyUpgradeReward UpgradeReward = EnemyUpgradeRewardOptions[RewardIndex];
	UUDEnemyUpgrade* EnemyUpgrade = UpgradeReward.UpgradeInstance;
	TSubclassOf<AUDEnemy> EnemyToUpgrade = UpgradeReward.EnemyUpgraded;

	//Find the enemy being upgraded in the spawn pool and set them to have the upgrade.
	for (int i = 0; i < KnightSpawnPool.Num(); i++)
	{
		//We get a pointer to the spawn entry since we will be editting it.
		FEnemySpawnEntry* SpawnEntry = &KnightSpawnPool[i];

		if (SpawnEntry->EnemyClass == EnemyToUpgrade)
		{
			SpawnEntry->Upgrade = EnemyUpgrade;
		}
	}

	//Make sure that the relavant enemy in the currently generated wave also gets the upgrade.
	for (int i = 0; i < CurrentRoundWaves.Num(); i++)
	{
		//Get a pointer to the wave being modified this iteration.
		FEnemyWave* Wave = &CurrentRoundWaves[i];

		//Make sure the wave contains the enemy we are upgrading.
		if (Wave->KnightParams.Contains(EnemyToUpgrade))
		{
			Wave->KnightParams[EnemyToUpgrade].Upgrade = EnemyUpgrade;
		}
	}

	//Enemies are only allowed one upgrade: iterate through the upgrade pool and remove any other upgrades that target that enemy.
	for (int i = 0; i < EnemyUpgradeRewardPool.Num();)
	{
		FEnemyUpgradeReward Upgrade = EnemyUpgradeRewardPool[i];

		if (Upgrade.EnemyUpgraded == EnemyToUpgrade)
		{
			EnemyUpgradeRewardPool.RemoveAt(i);
		}
		else
		{
			i++;
		}
	}

	CheckEnemyProgression();
}

void UUrsadeathGameInstance::AddUpgradeReward()
{
	int32 RewardIndex = UpgradeRewardMenu->GetRewardSelected();

	FPlayerUpgradeData* UpgradeData = UpgradeRewardOptions[RewardIndex];

	UUDPlayerUpgrade::CreatePlayerUpgrade(UpgradeData->UpgradeClass, PlayerCharacter);

	UpgradeRewardOptions.Remove(UpgradeData);

	//If the player starts with extra upgrades due to starting beyond Round 1, repopulate the upgrades so that the player has new to choose from.
	if (ExtraUpgrades > 0)
	{
		ExtraUpgrades--;

		DiscardIntoPool(UpgradeRewardPool, UpgradeRewardOptions);
		
		PopulateUpgradeRewards();

		if (ExtraUpgrades == 0)
		{
			UpgradeRewardMenu->SetRepeatReward(false);
		}
	}
}

void UUrsadeathGameInstance::SetupGame()
{
	//Initialize the game's random seed unless a seed was already input.
	if (GameSeedName == "" || SeedWasRandomized == true)
	{
		//Randomly generate the game's seed.
		RandomStream.GenerateNewSeed();

		//Set the displayed seed name to the randomly generated seed.
		GameSeedName = FName(FString::FromInt(RandomStream.GetCurrentSeed()));

		//Tell the game that the given seed was randomly generated. This causes a new random seed to be generated if the game is reset from the player dying.
		SeedWasRandomized = true;
	}
	else
	{
		//Initialize the game seed with the seed input by the player.
		RandomStream.Initialize(GameSeedName);
	}
	
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

	//Get an array of the enemy upgrades
	TArray<FEnemyUpgradeData*> EnemyUpgradeData;
	EnemyUpgradeDataTable->GetAllRows("GameInstanceEnemyUpgradeInit", EnemyUpgradeData);

	//Set the Player Upgrade Pool from the data table of Player Upgrades.
	PlayerUpgradeDataTable->GetAllRows("GameInstancePlayerRewardInit", UpgradeRewardPool);

	//Remove any disabled upgrades.
	for(int i = 0; i < UpgradeRewardPool.Num();)
	{
		FPlayerUpgradeData* UpgradeData = UpgradeRewardPool[i];

		if (!UpgradeData->bEnabled)
		{
			UpgradeRewardPool.Remove(UpgradeData);
		}
		else
		{
			i++;
		}
	}

	//Shuffle the player upgrades
	ShuffleArray(UpgradeRewardPool);

	//Shuffle the Knight rewards.
	ShuffleArray(KnightRewardPool);

	//Perform some extra setup if the game is starting on a Round/Wave other than the first.
	if (StartingRound != 0 || StartingWave != 0)
	{
		RoundWaveNumber = AbsoluteWaveNumber = StartingWave;

		RoundNumber = StartingRound;

		//Give the player extra upgrade if starting beyond the first round
		//ExtraUpgrades = StartingRound;
		
		//Make sure the absolute wave count lines up.
		for (int i = 0; i < StartingRound; i++)
		{
			AbsoluteWaveNumber += RoundWaveCounts[i];
		}

		int32 ExtraNewEnemies = 0;

		//Count the number of knight rewards to add.
		for (int i = 0; i < AbsoluteWaveNumber; i++)
		{
			if (EnemyWaveSchemes[i]->NewRoundType == ENewRoundType::NEW_ENEMY)
			{
				FEnemySpawnEntry SpawnEntry;
				SpawnEntry.EnemyClass = UnchosenKnightSpawnData[ExtraNewEnemies].EnemyClass;
				KnightSpawnPool.Add(SpawnEntry);

				ExtraNewEnemies++;

				//Add an extra upgrade for each New Enemy Round. We make sure i is not zero since, while the first round is supposed to be a new enemy round, a single upgrade is given to the player regardless of round type!
				if (i != 0)
				{
					ExtraUpgrades++;
				}

				NewRoundProgessions.Add(ENewRoundType::NEW_ENEMY);
			}
			else if (EnemyWaveSchemes[i]->NewRoundType == ENewRoundType::UPGRADE_ENEMY)
			{
				NewRoundProgessions.Add(ENewRoundType::UPGRADE_ENEMY);
			}
		}

		UnchosenEnemies = ExtraNewEnemies;
	}
}

void UUrsadeathGameInstance::FinalizePlayerSetup(AUDPlayerCharacter* Player)
{
	PlayerCharacter = Player;
	
	UUDRoundScreenWidget* RoundScreenWidget = PlayerCharacter->GetRoundScreenWidget();

	//Cache the reward menus.
	UpgradeRewardMenu = RoundScreenWidget->GetUpgradeRewardMenu();
	EnemyUpgradeRewardMenu = RoundScreenWidget->GetEnemyUpgradeRewardMenu();
	KnightRewardMenu = RoundScreenWidget->GetKnightRewardMenu();

	//Tell the Upgrad Menu that it is expected to give out more than one upgrade. 
	if (ExtraUpgrades > 0)
	{
		UpgradeRewardMenu->SetRepeatReward(true);
	}
 
	UpdateRoundScreen();

	PopulateKnightRewards();
	PopulateEnemyUpgradeRewards();
	PopulateUpgradeRewards();

	//Make sure the proper Enemy Reward menu is visible.
	if (NewRoundProgessions[0] == ENewRoundType::NEW_ENEMY)
	{
		KnightRewardMenu->SetVisibility(ESlateVisibility::Visible);
		EnemyUpgradeRewardMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		KnightRewardMenu->SetVisibility(ESlateVisibility::Collapsed);
		EnemyUpgradeRewardMenu->SetVisibility(ESlateVisibility::Visible);
	}
	
	Player->GetHUDWidget()->SetGameSeedText(FText::FromName(GameSeedName));
}

bool const UUrsadeathGameInstance::RoundReadyToStart()
{
	return NewRoundProgessions.Num() == 0;
}

#undef LOCTEXT_NAMESPACE



