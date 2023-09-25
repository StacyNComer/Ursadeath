// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerExplosiveProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UDPlayerCharacter.h"
#include "UDEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

#define ECC_PLAYER ECC_GameTraceChannel2

void AUDPlayerExplosiveProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		//Spawn the explosion particle FX
		if (ExplosionParticleSystem)
		{
			UNiagaraComponent* ExplosionParticles = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, ExplosionParticleSystem, GetActorLocation());
			//Adjust the size of the particle FX to be that of the explosion.
			ExplosionParticles->SetFloatParameter(FName("User.ExplosionRadius"), ExplosionRadius);
		}
		
		//THe explosion searches for Pawns (which all enemies are) and Players.
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PLAYER));

		//The actor directly hit by the explosive projectile is not affected by the explosion.
		TArray<AActor*> ignoredActors;
		ignoredActors.Init(OtherActor, 1);

		//Holds the enemies hit by the explosion.
		TArray<AActor*> OutActorsHit;

		//Test for enemies in the explosion's radius.
		UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), ExplosionRadius, objectTypes, nullptr, ignoredActors, OutActorsHit);

		//Damage enemies and players caught in the sphere.
		for (int i = 0; i < OutActorsHit.Num(); i++)
		{
			if(AUDEnemy* EnemyHit = Cast<AUDEnemy>(OutActorsHit[i]))
			{
				ApplyAttackToEnemy(EnemyHit, ExplosionStats);
			}
			else if (AUDPlayerCharacter* PlayerHit = Cast<AUDPlayerCharacter>(OutActorsHit[i]))
			{
				PlayerHit->DamagePlayer(ExplosionPlayerDamage);
			}

			//Show a message for each Actor the explosion detected if the explosion is being debugged.
			if(bExplosionDebug)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OutActorsHit[i]->GetActorNameOrLabel());
		}

#if WITH_EDITOR
		//Draw a sphere for debugging the explosion radius.
		if (bExplosionDebug)
			DrawDebugSphere(World, GetActorLocation(), ExplosionRadius, 12, FColor::Green, false, 1);
#endif
	}

	Super::NotifyActorBeginOverlap(OtherActor);
}