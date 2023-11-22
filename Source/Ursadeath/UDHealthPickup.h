// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDHealthPickup.generated.h"

class AUDPlayerCharacter;

/** The base class for health pickups. Health pickups are used by the player melee attacking them. Health Pickups are reusable; When used, they merely disappear until a cooldown elapses.*/
UCLASS(Abstract)
class URSADEATH_API AUDHealthPickup : public AActor
{
	GENERATED_BODY()
	
protected:
	/** When a pickup is used, it is set on cooldown for this many seconds.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Cooldown;

	/** The amount of seconds remaining before the pickup may be used by the player again*/
	UPROPERTY(BlueprintReadOnly)
		float CooldownTracker;

	/** Audio played when the player picks this item up.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<USoundBase> PickupSound;

public:	
	// Sets default values for this actor's properties
	AUDHealthPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Causes the pickup to be used by the given player. The pickup then becomes intangable and invisible until its cooldown is expires.*/
	UFUNCTION(BlueprintCallable)
		void UsePickup(AUDPlayerCharacter* UsingPlayer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
