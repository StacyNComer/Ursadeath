// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UDPlayerHUDWidget.h"
#include "UDPlayerCharacter.h"

void AUDPlayerController::BeginPlay()
{
	//Cahe the controlled player.
	ControlledPlayer = CastChecked<AUDPlayerCharacter>(GetPawn());

	//Create player hud
	PlayerHUDWidget = CreateWidget<UUDPlayerHUDWidget>(this, PlayerHUDWidgetClass);
	PlayerHUDWidget->AddToViewport();
	
	//Give the PlayerHUDWidget to the player so that they can update it.
	ControlledPlayer->InitializeHUDWidget(PlayerHUDWidget);
}