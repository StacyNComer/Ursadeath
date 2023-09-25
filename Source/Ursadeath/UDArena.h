// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDArena.generated.h"

class AUDEnemy;
class UStaticMeshComponent;

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

/* A structure storing the information about each wave.**/
USTRUCT(BlueprintType)
struct FEnemyWave
{
	GENERATED_BODY()

public:
	/** A map linking each enemy class to how many of that enemy should spawn during a wave. An enemy class should only exist here if more than 0 of it should spawn.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TSubclassOf<AUDEnemy>, int32> EnemyCounts;

	/** How many squire class enemies that spawn during a given wave. Once this is depleted, squires spawn at a slow rate.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	int32 SquireSpawns;

	/** The maximum Knight class enemies that may be in play at once.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxKnights;

	/** The maximum Squire class enemies that may be in play at once.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxSquires;
};


/** An actor holding the arena mesh as well as all the locations for enemy and health bobule spawns.*/
UCLASS()
class URSADEATH_API AUDArena : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ArenaMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> EnemySpawnRoot;

	/** An array of spawn points that do not have an enemy currently being spawned at them.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<USceneComponent>> FreeEnemySpawnPoints;

	TArray<OccupiedSpawnPoint*> OccupiedSpawnPoints;

	/** The current wave that the arena will try to spawn. Note that the data in this struct is modified as enemies are spawned.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEnemyWave CurrentWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<AUDEnemy>> SquireSpawnPool;

	/** While true, the arena will try and spawn the current wave.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSpawningWave;

	/** An array of Knight class enemies that the current wave can spawn.*/
	TArray<TSubclassOf<AUDEnemy>> KnightSpawnPool;

	int32 SquiresInPlay = 0;

	int32 KnightsInPlay = 0;

#if WITH_EDITORONLY_DATA
	/** Spawns whatever the current wave is set to on Begin Play.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
	bool bSpawnAtBeginPlay;
#endif

public:	
	// Sets default values for this actor's properties
	AUDArena();

private:
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

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

	/** Signal to the arena that a knight class enemy has died.*/
	UFUNCTION()
	void DecrementKnightsInPlay(AActor* EnemyDestroyed);

	/** Signal to the arena that a squire class enemy has died.*/
	UFUNCTION()
	void DecrementSquiresInPlay(AActor* EnemyDestroyed);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	/** Sets the current enemy wave and begins spawning it.*/
	void SetCurrentWave(FEnemyWave Wave);
};
