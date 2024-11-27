// Fill out your copyright notice in the Description page of Project Settings.


#include "UDCoreLibrary.h"
#include "UDEnemy.h"
#include "Kismet/GameplayStatics.h"

#define ECC_ENEMY ECC_GameTraceChannel4

TArray<AUDEnemy*> UUDCoreLibrary::GetNearestEnemies(const UObject* WorldContextObject, int32 Count, const FVector Location)
{
	//The enemies that are left for this method to find.
	int32 RemainingCount = Count;

		//Setup for sphere-cast
	//World object
	UWorld* const World = WorldContextObject->GetWorld();
	//The radius of the sphere cast. Doubles in size with each search until the MaxSearchRadius is surpassed.
	float SearchRadius = 1000;
	//Search for objects of the "Enemy" type
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_ENEMY));
	//Holds the enemies hit by the explosion.
	TArray<AActor*> OutActorsHit;
	//Any aotors that that are found are ignored in any future searches performed by the same method call.
	TArray<AActor*> ActorsFound;
	
	while (RemainingCount > 0 && SearchRadius <= 4000)
	{
		UKismetSystemLibrary::SphereOverlapActors(World, Location, SearchRadius, ObjectTypes, AUDEnemy::StaticClass(), ActorsFound, OutActorsHit);

		//We only bother sorting the actors if there are more actors than remaining enemies to find. Otherwise, we append the remaining enemies.
		if (OutActorsHit.Num() > RemainingCount)
		{
			//Sort the actors hit by their distance from the chosen location. We use Insertion Sort as we don't expect more than ~15 actors at a time.
			int32 i = 1;
			while (i < OutActorsHit.Num())
			{
				int32 j = i;

				double dist = FVector::Distance(Location, OutActorsHit[j]->GetActorLocation());

				while (j > 0 && FVector::Distance(Location, OutActorsHit[j - 1]->GetActorLocation()) > dist)
				{
					OutActorsHit.Swap(j, j - 1);
					--j;
				}

				++i;
			}

			for (int k = 0; k < RemainingCount; ++k)
			{
				ActorsFound.Add(OutActorsHit[k]);
			}

			RemainingCount = 0;
		}
		else
		{
			ActorsFound.Append(OutActorsHit);
			RemainingCount -= OutActorsHit.Num();
		}

		//Double the search radius each time we go through the array.
		SearchRadius *= 2;
	}

	//Cast each of the actors found to be an enemy and return them.
	TArray<AUDEnemy*> EnemiesFound;
	for (int i = 0; i < ActorsFound.Num(); ++i)
	{
		EnemiesFound.Add(CastChecked<AUDEnemy>(ActorsFound[i]));
	}

	return EnemiesFound;
}

