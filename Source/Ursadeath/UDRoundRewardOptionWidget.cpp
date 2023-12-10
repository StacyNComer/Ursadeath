// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundRewardOptionWidget.h"
#include "UDRoundRewardMenu.h"
#include "Components/Button.h"

void UUDRoundRewardOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RewardButton->OnClicked.AddDynamic(this, &UUDRoundRewardOptionWidget::NativeOnRewardSelected);
}

void UUDRoundRewardOptionWidget::NativeOnRewardSelected()
{
	OnRewardSelected();

	ContainingMenu->UpdateRewardOptionSelected(this);
}

UButton* const UUDRoundRewardOptionWidget::GetRewardButton()
{
	return RewardButton;
}

int32 UUDRoundRewardOptionWidget::GetRewardIndex()
{
	return RewardEntryIndex;
}

void UUDRoundRewardOptionWidget::SetContainingMenu(UUDRoundRewardMenu* Menu, int32 RewardIndex)
{
	ContainingMenu = Menu;

	RewardEntryIndex = RewardIndex;
}

