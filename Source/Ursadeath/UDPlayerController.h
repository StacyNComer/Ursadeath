// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UDPlayerController.generated.h"

class UUDPlayerHUDWidget;

/**
 * 
 */
UCLASS()
class URSADEATH_API AUDPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUDPlayerHUDWidget> PlayerHUDWidgetClass;

public:
	UPROPERTY(VisibleAnywhere, BLueprintReadOnly)
	TObjectPtr<UUDPlayerHUDWidget> PlayerHUDWidget;

protected:
	void BeginPlay() override;
};
