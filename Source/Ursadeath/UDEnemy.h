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

/** Represents the tier of enemy. Untiered enemies are summons, Squires are weak fodder type enemies, Knights are stronger elite enemies, and Champions are bosses.*/
UENUM(BlueprintType)
enum class EEnemyTier
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

public:
	/** A delegate invoked before an enemy is affected by an attack during ReceiveAttack. This delegate is used to modify the attack based on an enemy's strength/weakness toward it.*/
	UPROPERTY(BlueprintAssignable)
		FAttackReceivedSignature OnAttackRecieved;

	/** If true, the enemy is immune to dying.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug, meta = (ExposeOnSpawn = "true"))
		bool bUndieable;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<UNiagaraComponent> StunParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<USoundBase> DamageSound;

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
		int32 health;

	/** The remaining seconds an enemy has until they are no longer stunned. An enemy is considered to be stunned while this is > 0.*/
	UPROPERTY(BlueprintReadOnly, Category = Status)
		float StunTime;

	/** Controls the altered appearence enemies have while "spawning in".*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TObjectPtr<UMaterialInterface> SpawningMaterial;

	/** A reference to the enemy controller casted as an UDEnemyController.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDEnemyController> EnemyController;

	/** This enemy's tier. Enemies do not have a hard coded tier.*/
	EEnemyTier EnemyTier;

private:
	/** Stores this enemy's meshes for altering their materials when they spawn or finish spawning.*/
	TInlineComponentArray<UMeshComponent*> EnemyMeshes;

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

	/** Returns true if the enemy is currently stunned*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Status)
		bool IsStunned();

	/** Returns the amount of time this enemy should spend "spawning in"*/
	float GetSpawnTime();

	/** Returns the enemy's tier.*/
	EEnemyTier GetEnemyTier();

	/** Sets the enemy tier. This should be used when an enemy first spawns.*/
	void SetEnemyTier(EEnemyTier Tier);

protected:
	/** An event right after the enemy's spawn sequence ends. Use this to add additional effects to the enemy's spawn sequence completing from inside blueprints.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnSpawnSequenceEnd();

	/** Called whenever the enemy is hit by an attack that stuns them. Implement this event whenever the enemy being stunned requires additional behavior (such as interupting an ongoing attack).
	* WasAlreadyStunned returns true if the enemy was already under the effects of a stun from a different effect.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnStunned(bool WasAlreadyStunned);

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
