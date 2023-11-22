// Fill out your copyright notice in the Description page of Project Settings.


#include "UDHealthPickup.h"
#include "UDPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values
AUDHealthPickup::AUDHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set default cooldown
	Cooldown = 35;

	CooldownTracker = 0;
}

// Called when the game starts or when spawned
void AUDHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUDHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Decrement the pickup's cooldown.
	if (CooldownTracker > 0)
	{
		CooldownTracker -= DeltaTime;

		//The pickup becomes visible and tangable when the cooldown expires.
		if (CooldownTracker <= 0)
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);
		}
	}
}

void AUDHealthPickup::UsePickup(AUDPlayerCharacter* UsingPlayer)
{
	UsingPlayer->NotifyOnHealthPickupUsed();

	//Make the pickup hidden and intangable.
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	//Pickup FX
	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PickupSound);
	}
	
	//Put the pickup on cooldown.
	CooldownTracker = Cooldown;
}

