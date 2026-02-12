// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPauseMenuWidget.h"
#include "UDButton.h"

void UUDPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentBackButton = ResumeButton;
}

void UUDPauseMenuWidget::BackOutMenu()
{
	CurrentBackButton->OnClicked.Broadcast();
}