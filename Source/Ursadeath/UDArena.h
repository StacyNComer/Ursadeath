// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDEnemy.h"
#include "UrsadeathGameInstance.h"
#include "UDArena.generated.h"

class AUDEnemy;
class UStaticMeshComponent;
class AUDHealthPickup;
class UArrowComponent;
class UUrsadeathGameInstance;
struct FEnemyWave;

/** A struct for tracking how long until a given scene component's position is no longer occupied by a spawning enemy.*/
struct OccupiedSpawnPoint
{
	TObjectPtr<USceneComponent> SpawnPoint;
	float TimeUntilFree;

	OccupiedSpawnPoint(TObjectPtr<USceneComponent> SpawnPoint, float TimeUntilFree)
	{
		this->SpawnPoint = SpawnPoint;
		this->TimeUntilFree = TimeUntilFree;
	}
};

/** An actor holding the arena mesh as well as all the locations for enemy and health bobule spawns.*/
UCLASS(Abstract)
class URSADEATH_API AUDArena : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UStaticMeshComponent> ArenaMesh;

	/** The transform that the Health Pickups will spawn at. These are blue to distinguish them from Enemy Spawn Points.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<TObjectPtr<UArrowComponent>> HealthPickupSpawnPoints;

	/** The class of health pickup this arena will spawn.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AUDHealthPickup> HealthPickupClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<USceneComponent> EnemySpawnRoot;

	/** An array of spawn points that do not have an enemy currently being spawned at them.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<TObjectPtr<USceneComponent>> FreeEnemySpawnPoints;

	/** The current wave that the arena will try to spawn. Note that the data in this struct is modified as enemies are spawned.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FEnemyWave CurrentWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<AUDEnemy>> SquireSpawnPool;

	/** While true, the arena will try and spawn the current wave.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bSpawningWave;

	/** An array of enemies that have died and remain on the ground as corpses.*/
	TArray<TObjectPtr<AUDEnemy>> EnemyCorpses;

	/** A casted reference to the game instance.*/
	TObjectPtr<UUrsadeathGameInstance> UrsadeathGameInstance;

	TArray<OccupiedSpawnPoint*> OccupiedSpawnPoints;

	/** An array of Knight class enemies that the current wave can spawn.*/
	TArray<TSubclassOf<AUDEnemy>> KnightSpawnPool;

	/** The health pickups spawned by the arena.*/
	TArray<AUDHealthPickup*> HealthPickups;

	/** The amount of seconds left before a Squire spawns when SquireSlowSpawnActive returns true.*/
	float SquireSlowSpawnCooldownTracker;

	int32 SquiresInPlay = 0;

	float KnightsInPlay = 0;

	//The maximum amount of times the game should attempt to choose a knight type whose spawn scalar is <= to the remain free spawn points.
	int32 MaxKnightSpawnAttempts = 10;

#if WITH_EDITORONLY_DATA
	/** Spawns whatever the current wave is set to on Begin Play.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
		bool bSpawnAtBeginPlay;
#endif

public:	
	// Sets default values for this actor's properties
	AUDArena();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/** Returns true if the current wave is currently in its "Squire Slow Spawn" phase. This generally occurs when the current wave's "Squire Count" has been depleted but there are still Knight or Champion enemies in play.*/
	bool GetSquireSlowSpawnActive();

	/** Test if there are no non-Squire enemies remaining and if current wave has no enemies left to be spawned. If these are true, the wave stops trying to spawn anything. If there are no Squires in play, then game will also proceed to the next wave/round.*/
	void CheckWaveDepletion();

	UFUNCTION(BlueprintCallable)
	/** Spawns an enemy at a random free spawn point. This will not check if a there is actually a free place to spawn at.*/
	AUDEnemy* SpawnEnemy(TSubclassOf<AUDEnemy> EnemyClass);

	/** Returns true if there is at least one free spawn point for an enemy to spawn at.*/
	UFUNCTION(BlueprintCallable)
		bool SpawnPointFree();

	/** Returns true if the current wave has remaining knight class enemies to spawn and spawning a knight would not exceed it's max knights in play.*/
	UFUNCTION(BlueprintCallable)
		bool CanSpawnKnight();

	/** Returns true if the current wave has remaining squire class enemies to spawn and spawning a squire would not exceed its max squires in play.*/
	UFUNCTION(BlueprintCallable)
		bool CanSpawnSquire();

	/** Add the given enemy to the array of corpses.*/
	UFUNCTION()
		void AddEnemyCorpse(AUDEnemy* Corpse);

	/** Signal to the arena that a knight class enemy has died.*/
	UFUNCTION()
		void DecrementKnightsInPlay(AUDEnemy* EnemyDestroyed);

	/** Signal to the arena that a squire class enemy has died.*/
	UFUNCTION()
		void DecrementSquiresInPlay(AUDEnemy* EnemyDestroyed);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Sets the current enemy wave and begins spawning it.*/
	UFUNCTION(BlueprintCallable)
		void SpawnEnemyWave(FEnemyWave Wave);

	/** Returns the number of health pickups that are off cooldown and can be used by the player.*/
	UFUNCTION(BlueprintCallable)
		int32 GetActiveHealthPickups();

	/** Clears all of the corpses in game by deleting their*/
	UFUNCTION(BlueprintCallable)
		void ClearCorpses();

	/** Respawn any health pickups in the arena that are on cooldown.*/
	void ReactivateHealthPickups();

};
