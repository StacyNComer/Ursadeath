// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundRewardIconWidget.h"
#include "Components/Image.h"

void UUDRoundRewardIconWidget::SetLargeImage(UTexture2D* Image)
{
	RewardLargeImage = Image;

	LargeUIImage->SetBrushFromTexture(Image);
}

void UUDRoundRewardIconWidget::SetMiniImage(UTexture2D* Image)
{
	RewardMiniImage = Image;

	//We use "IsValid", since the pointer may be populated but invalid
	if (Image->IsValidLowLevel())
	{
		//Make sure that the image UI is visible.
		MiniUIImage->SetVisibility(ESlateVisibility::HitTestInvisible);

		MiniUIImage->SetBrushFromTexture(Image);
	}
	else
	{
		//Hide the image if no image is given (otherwise the player will just see a white square)
		MiniUIImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

UTexture2D* UUDRoundRewardIconWidget::GetLargeImage() const
{
	return RewardLargeImage;
}

UTexture2D* UUDRoundRewardIconWidget::GetMiniImage() const
{
	return RewardMiniImage;
}
