// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerAttackProjectile.h"
#include "UDPlayerExplosiveProjectile.generated.h"


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

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
		bool bShowExplosionRadius;
#endif

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
