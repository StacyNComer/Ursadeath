// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDPlayerAttack.generated.h"

class UUDPlayerAttackData;
class AUDEnemy;

/** An enum representing the general source of an attack.*/
UENUM(BlueprintType)
enum class EPlayerAttackType
{
	PRIMARY_FIRE = 0,
	MELEE = 1,
	ROCKET = 2,
	SHOCKWAVE = 3,
	UPGRADE = 4
};

/* Statistics on how an attack should affect a hit enemy. Anything in this structure is passed along to events that care about an attack's stats via the PlayerAttackDataClass.*/
USTRUCT(BlueprintType)
struct FPlayerAttackStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StunTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPlayerAttackType AttackType;
};

/* An actor meant to deliver a player attack to an enemy (e.g. a projectile). UDEnemy derived actors take damage when overlapping actors of this class.*/
UCLASS()
class URSADEATH_API AUDPlayerAttack : public AActor
{
	GENERATED_BODY()

protected:
	/** The name of the collision profile used for player attacks.*/
	const char* PlayerAttackCollisionProfile = "PlayerAttack";

	/** The stats for this attack. This is not meant to change after being initialized: Used the PlayerCharacter's OnAttackHit or Enemy's OnAttackReceived to alter the attack's stats.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FPlayerAttackStats AttackStats;

	/** The energy that should be given to the player if the attack hits an enemy.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float EnergyGain;
	
public:	
	// Sets default values for this actor's properties
	AUDPlayerAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Creates AttackData from this the given Attack stats struct and applies the attack to the enemy. When an enemy is attacked, the player owning the attack also gains the attacks EnergyGain.*/
	UFUNCTION(BlueprintCallable)
	void ApplyAttackToEnemy(AUDEnemy* Enemy, const FPlayerAttackStats AttackStatsStruct);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
