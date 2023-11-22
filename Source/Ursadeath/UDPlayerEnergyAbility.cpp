// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerEnergyAbility.h"
#include "UDPlayerCharacter.h"

UUDPlayerEnergyAbility::UUDPlayerEnergyAbility()
{
	//Set default energy cost. This should equal whatever 1 bar of energy is.
	EnergyCost = 100;
}

bool UUDPlayerEnergyAbility::CanUseAbility()
{
	return OwningPlayer->TestEnergyWithFX(EnergyCost);
}

void UUDPlayerEnergyAbility::NotifyOnAbilitySuccessful()
{
	Super::NotifyOnAbilitySuccessful();

	//Use the selected amount of energy 
	OwningPlayer->ExpendEnergy(EnergyCost);
}

