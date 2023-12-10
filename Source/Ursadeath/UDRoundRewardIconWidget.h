// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDRoundRewardIconWidget.generated.h"

class UImage;

/**
 * 
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundRewardIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> RewardImageUI;

	UTexture2D* RewardImage;

public:
	/** Set the image that this widget should show.*/
	void SetImageUI(UTexture2D* Image);

	/** Returns the image that this widget is currently displaying.*/
	UTexture2D* GetImage();
	
};
