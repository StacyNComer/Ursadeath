// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerExplosiveProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UDEnemy.h"

void AUDPlayerExplosiveProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		//All enemies are pawns, so it is safe to search for only pawns
		TArray< TEnumAsByte< EObjectTypeQuery > > objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		//The actor directly hit by the explosive projectile is not affected by the explosion.
		TArray<AActor*> ignoredActors;
		ignoredActors.Init(OtherActor, 1);

		//Holds the enemies hit by the explosion.
		TArray<AActor*> outEnemiesHit;

		//Test for enemies in the explosion's radius.
		UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), ExplosionRadius, objectTypes, AUDEnemy::StaticClass(), ignoredActors, outEnemiesHit);

		//Apply the explosion attack to each enemy caught in the sphere.
		for (int i = 0; i < outEnemiesHit.Num(); i++)
		{
			//The test only detects AUDEnemy derived actors, so this should never be null!
			AUDEnemy* enemyHit = Cast<AUDEnemy>(outEnemiesHit[i]);

			ApplyAttackToEnemy(enemyHit, ExplosionStats);
		}

#if WITH_EDITOR
		//Draw a sphere for debugging the explosion radius.
		if (bShowExplosionRadius)
			DrawDebugSphere(World, GetActorLocation(), ExplosionRadius, 12, FColor::Green, false, 1);
#endif
	}

	Super::NotifyActorBeginOverlap(OtherActor);
}