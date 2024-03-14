// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDPlayerAttack.generated.h"

class UUDPlayerAttackData;
class AUDEnemy;
class AUDPlayerCharacter;

/** An enum representing the general source of an attack.*/
UENUM(BlueprintType)
enum class EPlayerAttackType : uint8
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
		EPlayerAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StunTime;
};

/* An actor meant to deliver a player attack to an enemy (e.g. a projectile). UDEnemy derived actors take damage when overlapping actors of this class.*/
UCLASS(Abstract)
class URSADEATH_API AUDPlayerAttack : public AActor
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFinalizedSignature, AUDPlayerAttack*, AttackActor);

private:
	/** The player chracter that spawned this attack. This value is not set until it is needed the for first time: To get this value safely, use GetOwningPlayer()*/
	TObjectPtr<AUDPlayerCharacter> OwningPlayer;

protected:
	UPROPERTY(BlueprintAssignable)
		FOnAttackFinalizedSignature OnAttackFinalized;

	/** The name of the collision profile used for player attacks.*/
	const char* PlayerAttackCollisionProfile = "PlayerAttack";

	/** The energy that should be given to the player if the attack hits an enemy.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float EnergyGain;

	/** Holds the enemies that were hit by the ApplyAttackExclusive.*/
	TArray<AUDEnemy*> EnemiesHit;

public:
	/** The stats for this attack. This is not meant to change after being initialized: Used the PlayerCharacter's OnAttackHit or Enemy's OnAttackReceived to alter the attack's stats.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPlayerAttackStats AttackStats;

public:	
	/** Returns the player that spawned this projectile.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		AUDPlayerCharacter* const GetOwningPlayer();

	// Sets default values for this actor's properties
	AUDPlayerAttack();

protected:
	// Called when the game starts or when spawned. If an attack need to perform casts upon spawning (e.g. for an explosion or hitscan attack), FinalizeAttack should be used instead.
	virtual void BeginPlay() override;

	/** Creates AttackData from this the given Attack stats struct and applies the attack to the enemy. When an enemy is attacked, the player owning the attack also gains the attacks EnergyGain. 
	* Returns true if the attack killed the enemy.*/
	UFUNCTION(BlueprintCallable)
		void ApplyAttackToEnemy(AUDEnemy* Enemy, const FPlayerAttackStats AttackStatsStruct);

	/** Applies the attack to the given enemy, but only if the PlayerAttack has never called this method on the enemy.  
	
	* Note that enemies are excluded even if this method was previously called with a different AttackStats struct.*/
	UFUNCTION(BlueprintCallable)
	void ApplyAttackExclusive(AUDEnemy* Enemy, const FPlayerAttackStats AttackStatsStruct);

	/** Called after any post spawn modifications to an attack (such as from upgrades) are applied. If an attack needs to perfom any collision checks a effect enemies (e.g. a hitscan attack), the check should be performed with this function instead of Begin Play.*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnAttackFinalized"))
		void RecieveFinalizeAttack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** If the attack is hitscan or should otherwise performs collision casts upon spawning (like from an explosion), it should perform them with this event instead of Begin Play. This event called by the spawning player after they have completed any post-spawn modifications to the attack (i.e. from upgrades).*/
	virtual void FinalizeAttack();
};
