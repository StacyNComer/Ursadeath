// Fill out your copyright notice in the Description page of Project Settings.

#include "UDArena.h"
#include "UDEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "UrsadeathGameInstance.h"

// Sets default values
AUDArena::AUDArena()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the Arena's mesh.
	ArenaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArenaMesh"));

	//Create the EnemySpawnRoot and attach it to the arena mesh.
	EnemySpawnRoot = CreateDefaultSubobject<USceneComponent>(TEXT("EnemySpawnRoot"));
	EnemySpawnRoot->SetupAttachment(ArenaMesh);
}

void AUDArena::OnConstruction(const FTransform& Transform)
{
	//Set any component attached to the EnemySpawnRoot as a spawn point for enemies.
	FreeEnemySpawnPoints = EnemySpawnRoot->GetAttachChildren();
}

// Called when the game starts or when spawned
void AUDArena::BeginPlay()
{
	Super::BeginPlay();

	//Tell the game instance that this is the current arena.
	GetGameInstance<UUrsadeathGameInstance>()->GameArena = this;

#if WITH_EDITOR
	if (bSpawnAtBeginPlay)
	{
		SetCurrentWave(CurrentWave);
	}
#endif
}

// Called every frame
void AUDArena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Iterate through the occupied spawn points and free any whose enemies have already spawned.
	for (int i = 0; i < OccupiedSpawnPoints.Num();)
	{
		OccupiedSpawnPoint* SpawnPoint = OccupiedSpawnPoints[i];

		//Decrement the time until the spawn point is free.
		SpawnPoint->TimeUntilFree -= DeltaTime;

		//If the spawn point should be free, add it back to the array of free spawn points and remove it from the occupied spawn points.
		if (SpawnPoint->TimeUntilFree <= 0)
		{
			FreeEnemySpawnPoints.Add(SpawnPoint->SpawnPoint);

			OccupiedSpawnPoints.Remove(SpawnPoint);

			//Free the memory created by the occupied spawn point.
			delete SpawnPoint;
		}
		else
		{
			//The loop is only incremented if a spawn point was not remove, since TArrays automatically resize.
			i++;
		}
	}

	//Enemy Spawning
	if (bSpawningWave && SpawnPointFree())
	{
		//Spawning knight class enemies. The game spawns all the knights it possible can before trying to spawn Squires.
		if (CanSpawnKnight())
		{
			do
			{
				//Choose a random enemy class from the spawn pool to spawn.
				int EnemyClassIndex = FMath::RandRange(0, KnightSpawnPool.Num() - 1);
				TSubclassOf<AUDEnemy> EnemyClass = KnightSpawnPool[EnemyClassIndex];

				//Spawn the enemy
				AUDEnemy* EnemySpawned = SpawnEnemy(EnemyClass);

				//Decrement the amount of the spawned enemy type in the wave. If that enemy type is depleted, remove it from the wave and spawn pool so it is not chosen anymore.
				if (--CurrentWave.EnemyCounts[EnemyClass] == 0)
				{
					CurrentWave.EnemyCounts.Remove(EnemyClass);
					KnightSpawnPool.RemoveAt(EnemyClassIndex);
				}

				KnightsInPlay++;

				//Tell the enemy to signal the arena when it dies.
				EnemySpawned->OnDestroyed.AddDynamic(this, &AUDArena::DecrementKnightsInPlay);

			} while (CanSpawnKnight() && FreeEnemySpawnPoints.Num() > 0);
		}

		//Spawning Squire enemies. We check for a free spawn point here incase the knight spawn used them all.
		if (SpawnPointFree() && CanSpawnSquire())
		{
			do
			{
				//Choose a random enemy class from the spawn pool to spawn.
				TSubclassOf<AUDEnemy> EnemyClass = SquireSpawnPool[FMath::RandRange(0, SquireSpawnPool.Num() - 1)];

				AUDEnemy* EnemySpawned = SpawnEnemy(EnemyClass);

				CurrentWave.SquireSpawns--;

				SquiresInPlay++;

				EnemySpawned->OnDestroyed.AddDynamic(this, &AUDArena::DecrementSquiresInPlay);

			} while (CanSpawnSquire() && FreeEnemySpawnPoints.Num() > 0);
		}
	}

	//Stop spawning when the wave runs out of enemies.
	if (CurrentWave.EnemyCounts.Num() == 0 && CurrentWave.SquireSpawns == 0)
	{
		bSpawningWave = false;
	}
}

AUDEnemy* AUDArena::SpawnEnemy(TSubclassOf<AUDEnemy> EnemyClass)
{
	//Pick a random free spawn point.
	int SpawnIndex = FMath::RandRange(0, FreeEnemySpawnPoints.Num()-1);
	USceneComponent* SpawnPoint = FreeEnemySpawnPoints[SpawnIndex];

	UWorld* const World = GetWorld();

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//Add the spawn point to the array of occupied spawn points encased in an Occupied Spawn Points structure.
	OccupiedSpawnPoints.Add(new OccupiedSpawnPoint(SpawnPoint, EnemyClass.GetDefaultObject()->GetSpawnTime()));

	//Remove the used spawn point from the list of free enemy spawn points.
	FreeEnemySpawnPoints.RemoveAtSwap(SpawnIndex);

	//Spawn and return the enemy. Enemies automatically go through a "spawn sequence" when created.
	return World->SpawnActor<AUDEnemy>(EnemyClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), ActorSpawnParams);
}

void AUDArena::SetCurrentWave(FEnemyWave Wave)
{
	CurrentWave = Wave;

	//Generate the pool of Knight enemy types from the ones set to spawn in the enemy wave.
	CurrentWave.EnemyCounts.GenerateKeyArray(KnightSpawnPool);

	//Tell the arena to start spawning the current wave.
	bSpawningWave = true;
}

bool AUDArena::SpawnPointFree()
{
	return FreeEnemySpawnPoints.Num() > 0;
}

bool AUDArena::CanSpawnKnight()
{
	return CurrentWave.EnemyCounts.Num() > 0 && KnightsInPlay < CurrentWave.MaxKnights;
}

bool AUDArena::CanSpawnSquire()
{
	return CurrentWave.SquireSpawns > 0 && SquiresInPlay < CurrentWave.MaxSquires;
}

void AUDArena::DecrementKnightsInPlay(AActor* EnemyDestroyed)
{
	KnightsInPlay--;
}

void AUDArena::DecrementSquiresInPlay(AActor* EnemyDestroyed)
{
	SquiresInPlay--;
}