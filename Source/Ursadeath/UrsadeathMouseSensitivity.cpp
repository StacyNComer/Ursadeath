// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathMouseSensitivity.h"
#include "UrsadeathUserSettings.h"

FInputActionValue UUrsadeathMouseSensitivity::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	FVector2D Sensitivity = UUrsadeathUserSettings::GetUrsadeathUserSettings()->GetMouseSensitivity();

	return Sensitivity * CurrentValue.Get<FVector2D>();
}