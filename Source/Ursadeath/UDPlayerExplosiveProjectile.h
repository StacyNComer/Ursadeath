// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerAttackProjectile.h"
#include "UDPlayerExplosiveProjectile.generated.h"

class UNiagaraSystem;
class USoundBase;

/**
 * The base class for player projectiles that damages enemies in an area after hitting something. The actor directly hit by the projectile does not take the explosion's damage.
 */
UCLASS()
class URSADEATH_API AUDPlayerExplosiveProjectile : public AUDPlayerAttackProjectile
{
	GENERATED_BODY()

protected:
	/** The stats for this attack's explosion.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
		FPlayerAttackStats ExplosionStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
		float ExplosionRadius;

	/** The damage dealt to the player if they are caught in their own explosion.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
		int32 ExplosionPlayerDamage;

	/** A particle system spawned when the projectile explodes.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
		TObjectPtr<UNiagaraSystem> ExplosionParticleSystem;

	/** The sound played when this projectile detonates. Safe to omit.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<USoundBase> ExplosionSound;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
		bool bExplosionDebug;
#endif

protected:
	virtual void NotifyOnProjectileHit(AActor* ActorHit) override;
	
};
