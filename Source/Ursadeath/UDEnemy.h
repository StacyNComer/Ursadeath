// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UDEnemy.generated.h"

class UUDPlayerAttackData;
class UMeshComponent;

/*A delegate type for when the enemy is damaged.*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackReceived, UUDPlayerAttackData*, DamageData);

UCLASS()
class URSADEATH_API AUDEnemy : public APawn
{
	GENERATED_BODY()

public:
	/** A delegate invoked before an enemy is affected by an attack during ReceiveAttack. This delegate is used to modify the attack based on an enemy's strength/weakness toward it.*/
	UPROPERTY(BlueprintAssignable)
	FAttackReceived OnAttackRecieved;

	/** If true, the enemy is immune to dying.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	bool bUndieable;

protected:
	/** If true, the enemy won't undergo its spawning sequence. Instead, the enemy will recieve their default AI when play begins. Use for debug enemies without waiting for them to spawn.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	bool bSpawnInstantly;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	float SpawnTime = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
	int32 health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TObjectPtr<UMaterialInterface> SpawningMaterial;

private:
	/** Stores this enemy's meshes for altering their materials when they spawn or finish spawning*/
	TInlineComponentArray<UMeshComponent*> EnemyMeshes;

public:
	// Sets default values for this pawn's properties
	AUDEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Applies the given attack data to this enemy. The attacking player's OnAttackHit and this actor's own OnAttackReceived is called before the damage/stun is applied to the enemy. Returns true if the attack killed the enemy.*/
	UFUNCTION(BlueprintCallable, Category=Status)
	bool ReceiveAttack(UUDPlayerAttackData* AttackData);

	float GetSpawnTime();

protected:
	/** An event right after the enemy's spawn sequence ends. Use this to add additional effects to the enemy's spawn sequence completing from inside blueprints.*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnSpawnSequenceEnd();

private:
	/** A method called when the enemy finishes spawning. The OnSpawnSequenceEnd event can be used to add additional effects to the spawn sequence being completed from within blueprints.*/
	UFUNCTION()
	void EndSpawnSequence();

	/** Sets the material of all meshes in EnemyMeshes. If given a null material, this will set the meshes to their default materials similar to how AActor's SetMaterial method does.
	Meant to be used to set the material of the enmy mesh to or off from it's spawning material.*/
	void SetEnemyMaterials(UMaterialInterface* newMaterial);
};
