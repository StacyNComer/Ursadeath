// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathUserSettings.h"

UUrsadeathUserSettings::UUrsadeathUserSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

UUrsadeathUserSettings* UUrsadeathUserSettings::GetUrsadeathUserSettings()
{
	return Cast<UUrsadeathUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UUrsadeathUserSettings::ResetToDefaultSettings(bool bThisCannotBeReversedAreYouSure)
{
	if (bThisCannotBeReversedAreYouSure)
	{
		//Default settings
		ControllerSensitivity = FVector2D(.6f, .4f);

		MouseSensitivity = FVector2D(1, 1);

		bInvertYAxis = false;

		MasterVolume = 1;
		MusicVolume = 1;
		EnemySFXVolume = 1;
		PlayerSFXVolume = 1;

		//Reset the other user settings to default
		SetToDefaults();

		//Apply our settings.
		ApplySettings(false);
	}
}

FVector2D UUrsadeathUserSettings::GetControllerSensitivity()
{
	return ControllerSensitivity;
}

void UUrsadeathUserSettings::SetControllerSensitivity(FVector2D Sensitivity)
{
	ControllerSensitivity = Sensitivity;
}

FVector2D UUrsadeathUserSettings::GetMouseSensitivity()
{
	return MouseSensitivity;
}

void UUrsadeathUserSettings::SetMouseSensitivity(FVector2D Sensitivity)
{
	MouseSensitivity = Sensitivity;
}

bool UUrsadeathUserSettings::GetInvertYAxis()
{
	return bInvertYAxis;
}

void UUrsadeathUserSettings::SetInvertYAxis(bool bInvertY)
{
	bInvertYAxis = bInvertY;
}

float UUrsadeathUserSettings::GetMasterVolume()
{
	return MasterVolume;
}

void UUrsadeathUserSettings::SetMasterVolume(float Volume)
{
	MasterVolume = Volume;
}

float UUrsadeathUserSettings::GetPlayerSFXVolume()
{
	return PlayerSFXVolume;
}

void UUrsadeathUserSettings::SetPlayerSFXVolume(float Volume)
{
	PlayerSFXVolume = Volume;
}

float UUrsadeathUserSettings::GetEnemySFXVolume()
{
	return EnemySFXVolume;
}

void UUrsadeathUserSettings::SetEnemySFXVolume(float Volume)
{
	EnemySFXVolume = Volume;
}

float UUrsadeathUserSettings::GetMusicVolume()
{
	return MusicVolume;
}

void UUrsadeathUserSettings::SetMusicVolume(float Volume)
{
	MusicVolume = Volume;
}
