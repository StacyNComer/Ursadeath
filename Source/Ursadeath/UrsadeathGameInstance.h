// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UrsadeathGameInstance.generated.h"
 
class AUDEnemy;
class AUDArena;
struct FEnemyWave;

/**
 *
 */
UCLASS()
class URSADEATH_API UUrsadeathGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/* The current arena in play.**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AUDArena> GameArena;

protected:
	/* An array of waves that will spawn during the current round.**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEnemyWave> RoundWaves;
};