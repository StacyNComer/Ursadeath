// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathInvertAim.h"
#include "UrsadeathUserSettings.h"

FInputActionValue UUrsadeathInvertAim::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (UUrsadeathUserSettings::GetUrsadeathUserSettings()->GetInvertYAxis())
	{
		return FVector2D(CurrentValue.Get<FVector2D>().X, CurrentValue.Get<FVector2D>().Y * -1);
	}

	return CurrentValue.Get<FVector2D>();
}

