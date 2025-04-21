// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "UrsadeathUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class URSADEATH_API UUrsadeathUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(Config)
		FVector2D MouseSensitivity;

	UPROPERTY(Config)
		FVector2D ControllerSensitivity;

	UPROPERTY(Config)
		bool bInvertYAxis;

public:
	/** Returns a point to the object handling Ursadeath's settings.*/
	UFUNCTION(BlueprintCallable)
		static UUrsadeathUserSettings* GetUrsadeathUserSettings();

	/** Resets each and every setting to its default value and applies them. NO, THIS CANNOT BE UNDONE OR REVERSED!*/
	UFUNCTION(BlueprintCallable)
		void ResetToDefaultSettings(bool bThisCannotBeReversedAreYouSure);

	UFUNCTION(BlueprintCallable)
		FVector2D GetControllerSensitivity();

	UFUNCTION(BlueprintCallable)
		void SetControllerSensitivity(FVector2D Sensitivity);

	UFUNCTION(BlueprintCallable)
		FVector2D GetMouseSensitivity();

	UFUNCTION(BlueprintCallable)
		void SetMouseSensitivity(FVector2D Sensitivity);

	UFUNCTION(BlueprintCallable)
		bool GetInvertYAxis();

	UFUNCTION(BlueprintCallable)
		void SetInvertYAxis(bool bInvertY);
};
