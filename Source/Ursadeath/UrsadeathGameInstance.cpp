// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathGameInstance.h"
#include "UDEnemy.h"
#include "UDArena.h"
#include "UDPlayerCharacter.h"

void UUrsadeathGameInstance::Init()
{
	//Generate an array of the EnemySpawnData structures.
	TArray<FEnemySpawnData*> KnightSpawnData;
	EnemySpawnDataTable->GetAllRows("GameInstanceKnightSpawnDataMapInit", KnightSpawnData);

	//Populate the EnemyDataMap with the spawn data of each enemy.
	for (int i = 0; i < KnightSpawnData.Num(); i++)
	{
		FEnemySpawnData* SpawnDataEntry = KnightSpawnData[i];
		EnemyDataMap.Add(SpawnDataEntry->EnemyClass, SpawnDataEntry);
	}

	//Initialize the game's random seed.
	RandomStream.Initialize(GameSeedName);
}

void UUrsadeathGameInstance::StartWave(FEnemyWave Wave)
{
	PlayerCharacter->DisplayEnemyWave(Wave);
	GameArena->SetCurrentWave(Wave);
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

	//If UseNewKnight is set to true, add the NewKnightType to the wave. 
	if (WaveScheme.UseNewKnight)
	{
		EnemyWave.KnightCounts.Add(NewKnightType, 1);

		//Record that the Knight Type is in the wave for later use.
		WaveKnightTypes.Add(NewKnightType);

		KnightPool.Remove(NewKnightType);
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

FEnemySpawnData UUrsadeathGameInstance::GetSpawnDataEntry(TSubclassOf<AUDEnemy> EnemyClass)
{
	return *EnemyDataMap[EnemyClass];
}



