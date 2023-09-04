// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UDPlayerHUDWidget.h"

void AUDPlayerController::BeginPlay()
{
	//Create player hud
	PlayerHUDWidget = CreateWidget<UUDPlayerHUDWidget>(this, PlayerHUDWidgetClass);
	PlayerHUDWidget->AddToViewport();
}