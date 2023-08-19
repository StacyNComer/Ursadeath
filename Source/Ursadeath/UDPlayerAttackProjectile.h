// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerAttack.h"
#include "UDPlayerAttackProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

/**
 * The base class for player attacks that are projectiles. These are destroyed when they hit anything other than the player.
 */
UCLASS()
class URSADEATH_API AUDPlayerAttackProjectile : public AUDPlayerAttack
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

public:
	AUDPlayerAttackProjectile();

protected:
	/** By default this destroys the projectile.*/
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
