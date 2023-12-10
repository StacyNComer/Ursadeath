// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundRewardIconWidget.h"
#include "Components/Image.h"

void UUDRoundRewardIconWidget::SetImageUI(UTexture2D* Image)
{
	RewardImage = Image;

	RewardImageUI->SetBrushFromTexture(Image);
}

UTexture2D* UUDRoundRewardIconWidget::GetImage()
{
	return RewardImage;
}