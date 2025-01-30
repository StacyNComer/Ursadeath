// Fill out your copyright notice in the Description page of Project Settings.


#include "UDControllerButtonLink.h"
#include "UDPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUDControllerButtonLink::LinkPlayerController()
{
	PlayerController->LinkedButton = LinkedButton;
}

void UUDControllerButtonLink::UnlinkPlayerController()
{
	if (PlayerController->LinkedButton == LinkedButton)
	{
		PlayerController->LinkedButton = nullptr;
	}
}

void UUDControllerButtonLink::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerController = CastChecked<AUDPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	LinkedButton->OnHovered.AddDynamic(this, &UUDControllerButtonLink::LinkPlayerController);
	LinkedButton->OnUnhovered.AddDynamic(this, &UUDControllerButtonLink::UnlinkPlayerController);
}
