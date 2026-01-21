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
		TObjectPtr<UImage> LargeUIImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> MiniUIImage;

	UTexture2D* RewardLargeImage;

	UTexture2D* RewardMiniImage;

public:
	/** Set the large image that this widget should show.*/
	void SetLargeImage(UTexture2D* Image);

	/** Set the mini image that this widget should show. If no image is given, the image is hidden*/
	void SetMiniImage(UTexture2D* Image);

	/** Returns the large image that this widget is currently displaying.*/
	UTexture2D* GetLargeImage() const;

	/** Returns the smaller image that this widget is currently displaying.*/
	UTexture2D* GetMiniImage() const;
	
};
