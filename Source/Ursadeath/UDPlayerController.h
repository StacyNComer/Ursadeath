// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UDPlayerController.generated.h"

class UUDPlayerHUDWidget;
class UUDRoundScreenWidget;
class AUDPlayerCharacter;

/**
 * 
 */
UCLASS()
class URSADEATH_API AUDPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** The Player Character being controlled by this controller.*/
	TObjectPtr<AUDPlayerCharacter> ControlledPlayer;

protected:
	void BeginPlay() override;
};
