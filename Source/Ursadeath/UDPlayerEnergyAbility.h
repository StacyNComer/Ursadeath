// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerAbilityBase.h"
#include "UDPlayerEnergyAbility.generated.h"

/**
 * A component for storing the information of special player abilities that use the player's energy.
 */
UCLASS()
class URSADEATH_API UUDPlayerEnergyAbility : public UUDPlayerAbilityBase
{
	GENERATED_BODY()
	
protected:
	UUDPlayerEnergyAbility();

	/** How much energy that is needed to use this ability.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	float EnergyCost;

protected:

	bool CanUseAbility() override;

	void NotifyOnAbilitySuccessful() override;
};
