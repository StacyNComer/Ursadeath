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
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovementComponent;

public:
	AUDPlayerAttackProjectile();

private:
	/** A method that is binded to the SphereComponent's begin overlap delegate.*/
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
