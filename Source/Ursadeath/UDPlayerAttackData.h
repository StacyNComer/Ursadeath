// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerAttack.h"
#include "UObject/NoExportTypes.h"
#include "UDPlayerAttackData.generated.h"

/**
 * A class meant to box PlayerAttackStats for modification in EnemyDamaged events. UObject pointers have the special ability to be used with delegates without pissing them like other pointers do.
   This class is only meant to be used with damage events, and should be marked for garbage collection as soon as they are completed.
 */
UCLASS()
class URSADEATH_API UUDPlayerAttackData : public UObject
{
	GENERATED_BODY()

public:
	/** The stats of the player attack this instance was made from. The PlayerAttackData class contains several methods for easily modifying this value.*/
	UPROPERTY(BlueprintReadWrite)
	FPlayerAttackStats AttackStats;

public:
	/* Creates, initializes, and returns an instance of player attack data.*/
	static UUDPlayerAttackData* CreatePlayerAttackData(FPlayerAttackStats);
	
	/** Sets the damage value of this instance's AttackStats. Makes it easier to edit the struct in blueprints.*/
	UFUNCTION(Blueprintcallable)
	void SetDamage(int32 value);

	/** Sets the StunTime value of this instance's AttackStats. Makes it easier to edit the struct in blueprints.*/
	UFUNCTION(Blueprintcallable)
	void SetStunTime(float value);
};
