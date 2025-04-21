// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "UrsadeathInvertAim.generated.h"

/**
 * 
 */
UCLASS()
class URSADEATH_API UUrsadeathInvertAim : public UInputModifier
{
	GENERATED_BODY()

	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
	
};
