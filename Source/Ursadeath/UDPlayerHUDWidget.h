// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDPlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class URSADEATH_API UUDPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**Called by the player character whenever their health changes. Override to define how the HUD should change in response to the player's health.*/
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(int value);

	/**Called by the player character whenever their energy changes. Override to define how the HUD should change in response to the player's energy.*/
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnergy(float value);
};
