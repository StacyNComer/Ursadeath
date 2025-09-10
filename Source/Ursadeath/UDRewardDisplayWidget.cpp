// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRewardDisplayWidget.h"
#include "UDDescriptionSourceWidget.h"
#include "UDRoundRewardIconWidget.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UUDRewardDisplayWidget::NativeOnInitialized()
{
	RewardTitleText->SetText(RewardTitleMessage);
}

void UUDRewardDisplayWidget::InitDescriptionReceiver(TScriptInterface<IUDUIDescriptionReceiver> DescReceiver)
{
	DescriptionReceiver = DescReceiver;
}

void UUDRewardDisplayWidget::AddReward(UUDDescriptionSourceWidget* DescriptionSource, UUDRoundRewardIconWidget* RewardIconWidget)
{
	RewardDisplayPanel->AddChild(DescriptionSource);

	DescriptionSource->GetContentPanel()->AddChild(RewardIconWidget);
}
