// Fill out your copyright notice in the Description page of Project Settings.


#include "UDCoreLibrary.h"
#include "UDEnemy.h"
#include "Kismet/GameplayStatics.h"

#define ECC_ENEMY ECC_GameTraceChannel4

TArray<AUDEnemy*> UUDCoreLibrary::GetNearestEnemies(const UObject* WorldContextObject, TArray<AActor*> ActorsIgnored, const int32 Count, const int32 MaxSearchAttempts, const FVector Location, const bool bDrawDebug)
{
	//The enemies that are left for this method to find.
	int32 RemainingCount = Count;

	int32 SearchAttempts = 0;

		//Setup for sphere-cast
	//World object
	UWorld* const World = WorldContextObject->GetWorld();
	//The radius of the sphere cast. Doubles in size with each search until the MaxSearchRadius is surpassed.
	float SearchRadius = 1200;
	//Search for objects of the "Enemy" type
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_ENEMY));
	//Holds the enemies hit by the explosion.
	TArray<AActor*> OutActorsHit;
	
	//Any aotors that that are found are ignored in any future searches performed by the same method call.
	TArray<AActor*> ActorsFound;

	while (RemainingCount > 0 && SearchAttempts < MaxSearchAttempts)
	{
		UKismetSystemLibrary::SphereOverlapActors(World, Location, SearchRadius, ObjectTypes, AUDEnemy::StaticClass(), ActorsIgnored, OutActorsHit);

#if WITH_EDITOR
		if (bDrawDebug)
		{
			//Draw a sphere for debugging the explosion radius.
			DrawDebugSphere(World, Location, SearchRadius, 12, FColor::Green, false, 1 + SearchRadius / 2000);
		}
#endif

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
			//We only update Actors Ignored here since in the first case there will never be another iteration!
			ActorsIgnored.Append(OutActorsHit);

			RemainingCount -= OutActorsHit.Num();
		}

		SearchAttempts++;
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

float UUDCoreLibrary::GetArenaDiameter()
{
	return ArenaDiameter;
}
