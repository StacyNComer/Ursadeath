// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathControllerSensitivity.h"
#include "UrsadeathUserSettings.h"

FInputActionValue UUrsadeathControllerSensitivity::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	FVector2D Sensitivity = UUrsadeathUserSettings::GetUrsadeathUserSettings()->GetControllerSensitivity();

	return Sensitivity * CurrentValue.Get<FVector2D>();
}
