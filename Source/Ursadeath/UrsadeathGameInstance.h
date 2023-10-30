// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UrsadeathGameInstance.generated.h"
 
class AUDEnemy;
class AUDArena;
class AUDPlayerCharacter;
class UDataTable;
struct FEnemyWave;
struct FEnemySpawnData;

/**
 *
 */
UCLASS()
class URSADEATH_API UUrsadeathGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	/** An array of waves that will spawn during the current round.*/
	UPROPERTY(BlueprintReadOnly)
		TArray<FEnemyWave> RoundWaves;

	/** Maps the classes of Knight tier enemies in play to their entry in the data table so that they are easy for other parts of the program to get.*/
	TMap<TSubclassOf<AUDEnemy>, FEnemySpawnData*> KnightDataMap;

public:
	/** The maximum different classes of knight tier enemy that should ever be in a single wave.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnData)
		int MaxKnightTypesPerWave = 4;

	/** The maximum waves a round may ever have.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnData)
		int MaxWavesPerRound = 5;

	/* The current arena in play.**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<AUDArena> GameArena;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerCharacter> PlayerCharacter;

	/** This should contain a data table of all of the game's Knight tier enemies and any other information for spawning them.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyData)
		TObjectPtr<UDataTable> KnightSpawnDataTable;

	/** The icon used for squires in their spawn counter.*/
	UPROPERTY(EditAnywhere, Category = EnemyData)
		TObjectPtr<UTexture2D> SquireIcon;

protected:
	/** Tells the Arena to spawn the given wave and set the player to display the given wave to their UI.*/
	UFUNCTION(BlueprintCallable)
		void StartWave(FEnemyWave Wave);

	virtual void Init() override;

public:
	/** Returns the Spawn Data for a given class of enemy from the SpawnDataTable.*/
	UFUNCTION(BlueprintCallable)
		FEnemySpawnData GetSpawnDataEntry(TSubclassOf<AUDEnemy> EnemyClass);

};