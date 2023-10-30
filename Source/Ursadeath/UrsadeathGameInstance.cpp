// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathGameInstance.h"
#include "UDEnemy.h"
#include "UDArena.h"
#include "UDPlayerCharacter.h"

void UUrsadeathGameInstance::Init()
{
	TArray<FEnemySpawnData*> KnightSpawnData;
	KnightSpawnDataTable->GetAllRows("GameInstanceKnightSpawnDataMapInit", KnightSpawnData);

	for (int i = 0; i < KnightSpawnData.Num(); i++)
	{
		FEnemySpawnData* SpawnDataEntry = KnightSpawnData[i];
		KnightDataMap.Add(SpawnDataEntry->EnemyClass, SpawnDataEntry);
	}
}

void UUrsadeathGameInstance::StartWave(FEnemyWave Wave)
{
	PlayerCharacter->DisplayEnemyWave(Wave);
	GameArena->SetCurrentWave(Wave);
}

FEnemySpawnData UUrsadeathGameInstance::GetSpawnDataEntry(TSubclassOf<AUDEnemy> EnemyClass)
{
	return *KnightDataMap[EnemyClass];
}



