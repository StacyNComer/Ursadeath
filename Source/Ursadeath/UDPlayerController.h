// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UDPlayerController.generated.h"

class UUDPlayerHUDWidget;
class UUDRoundScreenWidget;
class AUDPlayerCharacter;
class UButton;

/**
 * 
 */
UCLASS()
class URSADEATH_API AUDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/**The UI button that will be pressed when the relevant input action on the controller is hit.
	* This system of UI interaction is temporary so I will not be encapsulating this.*/
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UButton> LinkedButton;

protected:
	/** The Player Character being controlled by this controller.*/
	TObjectPtr<AUDPlayerCharacter> ControlledPlayer;

protected:
	void BeginPlay() override;
};
