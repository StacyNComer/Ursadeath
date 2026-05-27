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

	UPROPERTY(Config)
		float MasterVolume;

	UPROPERTY(Config)
		float PlayerSFXVolume;

	UPROPERTY(Config)
		float EnemySFXVolume;

	UPROPERTY(Config)
		float MusicVolume;

public:
	/** Returns a point to the object handling Ursadeath's settings.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UUrsadeathUserSettings* GetUrsadeathUserSettings();

	/** Resets each and every setting to its default value and applies them. NO THIS CANNOT BE UNDONE OR REVERSED!*/
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
		float GetMasterVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetMasterVolume(float volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
		float GetPlayerSFXVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetPlayerSFXVolume(float volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
		float GetEnemySFXVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetEnemySFXVolume(float volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
		float GetMusicVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetMusicVolume(float volume);

};
