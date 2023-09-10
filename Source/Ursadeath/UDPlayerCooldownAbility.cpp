// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerCooldownAbility.h"

UUDPlayerCooldownAbility::UUDPlayerCooldownAbility()
{
	//Abilities do not tick by default.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentCooldown = 0;

	//Set base cooldown
	Cooldown = .15f;
}

void UUDPlayerCooldownAbility::ForceCooldown(float CooldownTime)
{
	CurrentCooldown = CooldownTime;
}

bool UUDPlayerCooldownAbility::CanUseAbility()
{
	return (CurrentCooldown <= 0);
}

void UUDPlayerCooldownAbility::NotifyOnAbilitySuccessful()
{
	Super::NotifyOnAbilitySuccessful();

	CurrentCooldown = Cooldown;
}

void UUDPlayerCooldownAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentCooldown > 0)
	{
		CurrentCooldown -= DeltaTime;
	}
}

