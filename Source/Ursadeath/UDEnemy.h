// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "UDEnemy.generated.h"

class UUDPlayerAttackData;
class UMeshComponent;
class AUDEnemyController;
class AUDEnemy;
class AUDPlayerCharacter;
class UNiagaraComponent;
class USoundBase;
class UFloatingPawnMovement;
class UUDEnemyAnimInstance; 
class UPrimitiveComponent;
class UCapsuleComponent;

/** Represents the tier of enemy. Untiered enemies are summons, Squires are weak fodder type enemies, Knights are stronger elite enemies, and Champions are bosses.*/
UENUM(BlueprintType)
enum class EEnemyTier : uint8
{
	UNTIERED = 0,
	SQUIRE = 1,
	KNIGHT = 2,
	CHAMPION = 3
};

UCLASS(Abstract)
class URSADEATH_API AUDEnemy : public APawn
{
	GENERATED_BODY()

	/*A delegate type for when the enemy is damaged.*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackReceivedSignature, UUDPlayerAttackData*, DamageData);

	/*A delegate type used for when the enemy is killed. Because we allow the enemy's corpse to remain, OnDestroyed is not suitable for tracking enemy deaths.*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyKilledSignature, AUDEnemy*, EnemyKilled);

	/** True if the enemy has finished spawning and is not dead. This is meant to be checked via the IsValidEnemy method.*/
	bool ValidEnemy = false;

public:
	/** A delegate invoked before an enemy is affected by an attack during ReceiveAttack. This delegate is used to modify the attack based on an enemy's strength/weakness toward it.*/
	UPROPERTY(BlueprintAssignable)
		FAttackReceivedSignature OnAttackRecieved;

	UPROPERTY(BlueprintAssignable)
		FEnemyKilledSignature OnEnemyKilled;

	/** If true, the enemy is immune to dying.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug, meta = (ExposeOnSpawn = "true"))
		bool bUndieable;

#if WITH_EDITORONLY_DATA
	/** If true, print any damage/stun that the enemy takes and their remaining health. Useful for if the damage/stun values of the player's attacks are acting funny.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug, meta = (ExposeOnSpawn = "true"))
		bool PrintDamageTaken;
#endif

	float EnemyCountWeight;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<USceneComponent> SceneRoot;

	/** The default collision capsule enemies have.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UCapsuleComponent> EnemyCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<UNiagaraComponent> StunParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<UNiagaraComponent> SlowParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<USoundBase> DamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<USoundBase> DeathSound;

	/** The spawn radius of the stun particle effect. 
	*
	* User parameters for Niagara Components don't appear in the detail panel if the component is made in C++ and all I got from looking this issue up was that this is a bug...*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		float StunParticleRadius = 75;

	/** If true, the enemy won't undergo its spawning sequence. Instead, the enemy will recieve their default AI when play begins. Use for debug enemies without waiting for them to spawn.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	bool bSpawnInstantly;

	/** The time in seconds that it takes for an enemy to "Spawn In".*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnTime = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		int32 Health;

	/** The remaining seconds an enemy has until they are no longer stunned. An enemy is considered to be stunned while this is > 0.*/
	UPROPERTY(BlueprintReadOnly, Category = Status)
		float StunTime;

	/** The remaining seconds an enemy has untile they are no longer slowed. An enemy is considered to be slowed while this is > 0*/
	UPROPERTY(BlueprintReadOnly, Category = Status)
		float SlowTime;

	/** The scalar applied to the enemy's movement speed while they are slowed
	    Not that slowing will not function properly if this value is 0.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float SlowedSpeedScalar;

	/** Controls the altered appearence enemies have while "spawning in".*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TObjectPtr<UMaterialInterface> SpawningMaterial;

	/** A reference to the enemy controller casted as an UDEnemyController.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDEnemyController> EnemyController;

	/** The enemy's animation instance. This may be null if either the enemy either lacks a skeletal mesh or if the skeletal mesh's animation blueprint isn't a child of the UDEnemyAnimInstance class.*/
	UPROPERTY(BlueprintReadOnly)
		UUDEnemyAnimInstance* EnemyAnimInstance;

	/** This enemy's tier. Enemies do not have a hard coded tier.*/
	EEnemyTier EnemyTier;

private:
	/** Stores this enemy's meshes for altering their materials when they spawn or finish spawning.*/
	TInlineComponentArray<UMeshComponent*> EnemyMeshes;

	/** Holds the enemy's floating movement component, if it has one.*/
	UFloatingPawnMovement* FloatingPawnMovement;

public:
	// Sets default values for this pawn's properties.
	AUDEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Applies the given attack data to this enemy. The attacking player's OnAttackHit and this actor's own OnAttackReceived is called before the damage/stun is applied to the enemy.
	* AttackSource is safe to omit if such needs to be done for debug/testing. Just note that this will cause the UI to not update!*/
	UFUNCTION(BlueprintCallable, Category=Status)
		void ReceiveAttack(UUDPlayerAttackData* AttackData, AUDPlayerAttack* AttackSource);

	/** Returns true if the enemy is currently stunned.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Status)
		const bool IsStunned();

	/** Slow the enemy's movement speed by the enemy's Slow Speed Scalar for the given amount of time. 
	If the enemy is already slowed, then it's remaining time slowed is set to the given time if the given time is longer than the remaining slow time.*/
	UFUNCTION(BlueprintCallable, Category = Status)
		void ApplySlow(float Time);

	/** Returns true if the enemy is currently slowed.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Status)
		const bool IsSlowed();

	/** Returns the enemy's tier. Squires are "fodder" enemies, */
	UFUNCTION(BlueprintCallable, BlueprintPure)
		EEnemyTier GetEnemyTier();

	/** Return's true if the enemy has no health and isn't immune to dying.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		const bool IsDead();

	/** Returns true if the enemy has finished spawning and is not dead.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsValidEnemy() const;

	/** Returns the amount of time this enemy should spend "spawning in"*/
	const float GetSpawnTime();

	/** Sets the enemy tier. This should be used when an enemy first spawns.*/
	void SetEnemyTier(EEnemyTier Tier);

protected:
	/** An event right after the enemy's spawn sequence ends. Use this to add additional effects to the enemy's spawn sequence completing from inside blueprints.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnSpawnSequenceEnd();

	/** Called whenever the enemy's actions are disrupted, such as by them being stunned or dying. Implement this event whenever the enemy being stunned/killed requires additional behavior (such as interupting an ongoing attack).
	* WasAlreadyStunned returns true if the enemy was already under the effects of a stun from a different effect.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnInterrupted(bool WasAlreadyStunned);

	/** Stuns the enemy for the given time. If the enemy is already stunned, the remaining time stunned will be set to TimeStunned if it is higher.*/
	void ApplyStun(float TimeStunned);

	virtual void PossessedBy(AController* NewController);

private:
	/** A method called when the enemy finishes spawning. The OnSpawnSequenceEnd event can be used to add additional effects to the spawn sequence being completed from within blueprints.*/
	UFUNCTION()
	void EndSpawnSequence();

	/** Sets the material of all meshes in EnemyMeshes. If given a null material, this will set the meshes to their default materials similar to how AActor's SetMaterial method does.
	Meant to be used to set the material of the enmy mesh to or off from it's spawning material.*/
	void SetEnemyMaterials(UMaterialInterface* newMaterial);

	friend AUDEnemyController;
};
