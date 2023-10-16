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

public:
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

};