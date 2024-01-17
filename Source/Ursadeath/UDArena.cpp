// Fill out your copyright notice in the Description page of Project Settings.

#include "UDArena.h"
#include "UDEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "UrsadeathGameInstance.h"
#include "UDHealthPickup.h"
#include "Components/ArrowComponent.h"

// Sets default values
AUDArena::AUDArena()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the Arena's mesh.
	ArenaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArenaMesh"));

	//Create the EnemySpawnRoot and attach it to the arena mesh.
	EnemySpawnRoot = CreateDefaultSubobject<USceneComponent>(TEXT("EnemySpawnRoot"));
	EnemySpawnRoot->SetupAttachment(ArenaMesh);

	for (int i = 0; i < 2; i++)
	{
		FString SpawnPointName = "HealthPickupSpawnPoint";
		SpawnPointName.AppendInt(i);

		UArrowComponent* HealthPickupSpawnPoint = CreateDefaultSubobject<UArrowComponent>(FName(*SpawnPointName));
		HealthPickupSpawnPoint->SetupAttachment(ArenaMesh);

		HealthPickupSpawnPoint->SetArrowFColor(FColor::Blue);

		HealthPickupSpawnPoints.Add(HealthPickupSpawnPoint);
	}
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

	UrsadeathGameInstance = GetGameInstance<UUrsadeathGameInstance>();
	UrsadeathGameInstance->GameArena = this;

	//Spawn a health pickup at each spawn point.
	for(int i = 0; i < HealthPickupSpawnPoints.Num(); i++)
	{
		FActorSpawnParameters ActorSpawnParams;
		//Set Spawn Collision Handling Override.
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//Set the attack's owner.
		ActorSpawnParams.Owner = this;

		//Get the spawn point for the pickup to spawn at.
		UArrowComponent* SpawnPoint = HealthPickupSpawnPoints[i];

		//Spawn the pickup.
		AUDHealthPickup* HealthPickup = GetWorld()->SpawnActor<AUDHealthPickup>(HealthPickupClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), ActorSpawnParams);

		//Add the health pickup to this arena's array of health pickups.
		HealthPickups.Add(HealthPickup);
	}

#if WITH_EDITOR
	if (bSpawnAtBeginPlay)
	{
		SpawnEnemyWave(CurrentWave);
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
			//The loop is only incremented if a spawn point was not removed, since TArrays automatically resize.
			i++;
		}
	}

	//Decrement the Slow Squire Spawn Cooldown, but only if the Squire Slow Spawn is active and only if a Squire is otherwise qualified to spawn in those circumstances.
	if (GetSquireSlowSpawnActive() && SquiresInPlay < CurrentWave.WaveData.MaxSlowSpawnSquires && SquireSlowSpawnCooldownTracker > 0)
	{
		SquireSlowSpawnCooldownTracker -= DeltaTime;
	}

	//Enemy Spawning
	if (bSpawningWave && SpawnPointFree())
	{
		//Spawning knight class enemies. The game spawns all the knights it possible can before trying to spawn Squires.
		if (CanSpawnKnight())
		{
			//Infinite loop prevention, just in case all Knight Types have a spawn scalar greater than 1 and there is only 1 free spawn point.
			int32 SpawnAttempts = 0;

			do
			{
				int32 EnemySpawnScalar;

				int32 EnemyClassIndex;
				TSubclassOf<AUDEnemy> EnemyClass;
				
				//Choose a random enemy class from the spawn pool to spawn. The game will
				do
				{
					EnemyClassIndex = FMath::RandRange(0, KnightSpawnPool.Num() - 1);
					EnemyClass = KnightSpawnPool[EnemyClassIndex];

					EnemySpawnScalar = UrsadeathGameInstance->GetSpawnDataEntry(EnemyClass).SpawnScalar;
					
					SpawnAttempts++;
				} while (EnemySpawnScalar > FreeEnemySpawnPoints.Num() && SpawnAttempts < MaxKnightSpawnAttempts);

				//Give up on spawning a knight if it is the last spawn attempt and a Knight that there is room enough to spawn still hasn't been found.
				if (SpawnAttempts == MaxKnightSpawnAttempts && EnemySpawnScalar > FreeEnemySpawnPoints.Num())
					break;

				//How much the enemy counts as for the purposes of determining the number of knights in play. A weight of 1 represents the enemy counting as a single enemy.
				float EnemyCountWeight = 1.0f / EnemySpawnScalar;

				//Spawn the enemy
				for (int i = 0; i < EnemySpawnScalar; i++)
				{
					AUDEnemy* EnemySpawned = SpawnEnemy(EnemyClass);

					//Set the enemy to be a Knight. ARISE A KNIGHT!
					EnemySpawned->SetEnemyTier(EEnemyTier::KNIGHT);

					EnemySpawned->EnemyCountWeight = EnemyCountWeight;

					//Tell the enemy to signal the arena when it dies.
					EnemySpawned->OnDestroyed.AddDynamic(this, &AUDArena::DecrementKnightsInPlay);
				}

				//Decrement the amount of the spawned enemy type in the wave. If that enemy type is depleted, remove it from the wave and spawn pool so it is not chosen anymore.
				if (--CurrentWave.KnightCounts[EnemyClass] == 0)
				{
					CurrentWave.KnightCounts.Remove(EnemyClass);
					KnightSpawnPool.RemoveAt(EnemyClassIndex);
				}

				KnightsInPlay++;

			} while (CanSpawnKnight() && FreeEnemySpawnPoints.Num() > 0);
		}

		//Spawning Squire enemies. We check for a free spawn point here incase the knight spawn used them all.
		if (SpawnPointFree() && CanSpawnSquire())
		{
			do
			{
				//Choose a random enemy class from the spawn pool to spawn.
				TSubclassOf<AUDEnemy> EnemyClass = SquireSpawnPool[FMath::RandRange(0, SquireSpawnPool.Num() - 1)];

				//Spawn the enemy
				AUDEnemy* EnemySpawned = SpawnEnemy(EnemyClass);

				//Set the enemy tier to be a squire.
				EnemySpawned->SetEnemyTier(EEnemyTier::SQUIRE);

				//If the wave is slow spawning squires, set the squire spawning on cooldown. Otherwise, decrement the squire count.
				if (GetSquireSlowSpawnActive())
				{
					SquireSlowSpawnCooldownTracker = CurrentWave.WaveData.SquireSlowSpawnCooldown;
				}
				else
				{
					CurrentWave.WaveData.SquireCount--;
				}

				SquiresInPlay++;

				//Tell the enemy to signal the arena when it dies.
				EnemySpawned->OnDestroyed.AddDynamic(this, &AUDArena::DecrementSquiresInPlay);

			} while (CanSpawnSquire() && FreeEnemySpawnPoints.Num() > 0);
		}
	}
}

bool AUDArena::GetSquireSlowSpawnActive()
{
	return KnightsInPlay > 0 && CurrentWave.WaveData.SquireCount == 0;
}

void AUDArena::CheckWaveDepletion()
{
	if (CurrentWave.WaveData.SquireCount == 0 && CurrentWave.KnightCounts.Num() == 0 && KnightsInPlay == 0)
	{
		bSpawningWave = false;

		if (SquiresInPlay == 0)
		{
			UrsadeathGameInstance->ProcessEndWave();
		}
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

void AUDArena::SpawnEnemyWave(FEnemyWave Wave)
{
	CurrentWave = Wave;

	//Generate the pool of Knight enemy types from the ones set to spawn in the enemy wave.
	CurrentWave.KnightCounts.GenerateKeyArray(KnightSpawnPool);

	//THe squire slow spawning starts on cooldown.
	SquireSlowSpawnCooldownTracker = CurrentWave.WaveData.SquireSlowSpawnCooldown;

	//Tell the arena to start spawning the current wave.
	bSpawningWave = true;
}

bool AUDArena::SpawnPointFree()
{
	return FreeEnemySpawnPoints.Num() > 0;
}

bool AUDArena::CanSpawnKnight()
{
	//We round up the decimal when comparing the knights in play to the max knight count: Otherwise, killing a single Knight type that spawns in pairs could spawn enemies above the Max Knights!
	return CurrentWave.KnightCounts.Num() > 0 && FMath::CeilToInt(KnightsInPlay) < CurrentWave.WaveData.MaxKnights;
}

bool AUDArena::CanSpawnSquire()
{
	if (GetSquireSlowSpawnActive())
	{
		return SquiresInPlay < CurrentWave.WaveData.MaxSlowSpawnSquires && SquireSlowSpawnCooldownTracker <= 0;
	}
	else
	{
		return CurrentWave.WaveData.SquireCount > 0 && SquiresInPlay < CurrentWave.WaveData.MaxSquires;
	}
	
}

void AUDArena::DecrementKnightsInPlay(AActor* EnemyDestroyed)
{
	KnightsInPlay -= CastChecked<AUDEnemy>(EnemyDestroyed)->EnemyCountWeight;

	CheckWaveDepletion();
}

void AUDArena::DecrementSquiresInPlay(AActor* EnemyDestroyed)
{
	SquiresInPlay--;

	CheckWaveDepletion();
}

int32 AUDArena::GetActiveHealthPickups()
{
	int32 ActivePickups = 0;

	for (int i = 0; i < HealthPickups.Num(); i++)
	{
		if (HealthPickups[i]->GetPickupActive())
		{
			ActivePickups++;
		}
	}

	return ActivePickups;
}

void AUDArena::ReactivateHealthPickups()
{
	for (int i = 0; i < HealthPickups.Num(); i++)
	{
		HealthPickups[i]->ReactivatePickup();
	}
}