// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDControllerButtonLink.generated.h"

class AUDPlayerController;
class UButton;

/**
 * A class made to give buttons the ability to be clicked via InputActions so long as the mouse cursor is above it.
 * The actual button must be set in the widget blueprint for this to work.
 * 
 * This a temporary solution to controller UI which I plan to later discard for a more traditional controller UI setup.
 * Thus the implementation will be cobbled together and sloppy!
 */
UCLASS()
class URSADEATH_API UUDControllerButtonLink : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<AUDPlayerController> PlayerController;

	/** The buttona that should */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UButton> LinkedButton;
	
private:
	UFUNCTION()
	void LinkPlayerController();

	/*If the button was already linked to player controller, unlink it.*/
	UFUNCTION()
	void UnlinkPlayerController();

protected:
	void NativeOnInitialized() override;
};
