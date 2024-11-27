// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerExplosiveProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UDPlayerCharacter.h"
#include "UDEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

//Alias the collision channels so that us humans know what is (or at least should) be tested.
#define ECC_PLAYER ECC_GameTraceChannel2
#define ECC_ENEMY ECC_GameTraceChannel4

void AUDPlayerExplosiveProjectile::NotifyOnProjectileHit(AActor* ActorHit)
{
	Super::NotifyOnProjectileHit(ActorHit);

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

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSound);
		}
		
		//The explosion searches for Enemies and Players.
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_ENEMY));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PLAYER));

		//The actor directly hit by the explosive projectile is not affected by the explosion.
		TArray<AActor*> ignoredActors;
		ignoredActors.Init(ActorHit, 1);

		//Holds the enemies hit by the explosion. 
		TArray<AActor*> OutActorsHit;

		//Test for enemies in the explosion's radius.
		UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), ExplosionRadius, objectTypes, nullptr, ignoredActors, OutActorsHit);

		//Damage enemies and players caught in the sphere.
		for (int i = 0; i < OutActorsHit.Num(); i++)
		{
			if(AUDEnemy* EnemyHit = Cast<AUDEnemy>(OutActorsHit[i]))
			{
				ApplyAttackExclusive(EnemyHit, ExplosionStats);
			}
			else if (AUDPlayerCharacter* PlayerHit = Cast<AUDPlayerCharacter>(OutActorsHit[i]))
			{
				PlayerHit->DamagePlayer(ExplosionPlayerDamage);
			}

#if WITH_EDITOR
			//Show a message for each Actor the explosion detected if the explosion is being debugged.
			if(bExplosionDebug)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OutActorsHit[i]->GetActorNameOrLabel());
#endif
		}

#if WITH_EDITOR
		//Draw a sphere for debugging the explosion radius.
		if (bExplosionDebug)
			DrawDebugSphere(World, GetActorLocation(), ExplosionRadius, 12, FColor::Green, false, 1);
#endif
	}
}