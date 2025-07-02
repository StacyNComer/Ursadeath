// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDPauseMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class URSADEATH_API UUDPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<UButton> ResumeButton;

	UPROPERTY(BlueprintReadWrite)
		TObjectPtr<UButton> CurrentBackButton;

protected:
	virtual void NativeOnInitialized() override;

public:
	/** Presses whatever the CurrentBackButton is set to.*/
	UFUNCTION(BlueprintCallable)
		void BackOutMenu();
};
