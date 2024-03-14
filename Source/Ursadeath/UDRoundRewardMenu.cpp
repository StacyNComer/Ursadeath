// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundRewardMenu.h"
#include "UDRoundRewardOptionWidget.h"
#include "UDRoundRewardIconWidget.h"
#include "UDDescriptionSourceWidget.h"
#include "UDUIDescriptionReceiver.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

#define LOCTEXT_NAMESPACE "RoundRewards"

void UUDRoundRewardMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Set the Reward's title message.
	RewardTitleText->SetText(RewardTitleMessage);

	//The confirm button for the rewards menu starts disable.
	ConfirmRewardButton->SetIsEnabled(false); 

	ConfirmRewardButton->OnClicked.AddDynamic(this, &UUDRoundRewardMenu::ConfirmReward);

	//Create the reward option entries
	for (int i = 0; i < MaxRewardOptions; i++)
	{
		FRewardOptionEntry RewardOptionEntry;

		//Create the description source.
		UUDDescriptionSourceWidget* DescriptionSource = WidgetTree->ConstructWidget<UUDDescriptionSourceWidget>(DescriptionSourceClass);
		//Add the description source as a child of the Reward Option Panel.
		RewardOptionPanel->AddChild(DescriptionSource);
		
		RewardOptionEntry.DescriptionSource = DescriptionSource;

		//Create the reward option widget.
		UUDRoundRewardOptionWidget* RewardOptionWidget = WidgetTree->ConstructWidget<UUDRoundRewardOptionWidget>(RewardOptionWidgetClass);
		//Set this as the containing menu for the Reward Option and tell it its index in the menu.
		RewardOptionWidget->SetContainingMenu(this, i);
		//Add the Reward Option Widget as a child of the Description Source.
		DescriptionSource->GetContentPanel()->AddChild(RewardOptionWidget);

		RewardOptionEntry.RewardOptionWidget = RewardOptionWidget;

		RewardOptionEntries.Add(RewardOptionEntry);
	}
}

void UUDRoundRewardMenu::ConfirmReward()
{
	ConfirmRewardButton->SetIsEnabled(false);

	//Prevent the reward options from being selected now that they are accepted.
	SetRewardOptionsEnabled(false);

	//Set the prompt text to show that the player has accepted their reward.
	PromptText->SetText(RewardAcceptedMessage);

	//Get the Reward Option Widget the player just accepted.
	FRewardOptionEntry RewardOptionEntryAccepted = RewardOptionEntries[RewardIndexSelected];

	//Create a Description Source for the reward the player accepted.
	UUDDescriptionSourceWidget* DescSource = CreateWidget<UUDDescriptionSourceWidget>(this, DescriptionSourceClass);	
	//Set the Description Source's receiver.
	DescSource->DescriptionReceiver = DescriptionReceiver;
	//Set the Description Source description to that of the reward Option the player had chosen.
	DescSource->SetDescription(RewardOptionEntryAccepted.DescriptionSource->GetDescription());
	//Attach the description to the Rewards Accepted Panel.
	RewardsAcceptedPanel->AddChild(DescSource);

	//Create a widget to display the reward the player accepted.
	UUDRoundRewardIconWidget* AcceptedRewardWidget = CreateWidget<UUDRoundRewardIconWidget>(this, RewardAcceptedWidgetClass);
	//Set the accepted reward's image to that of the Reward Option the player had chosen.
	AcceptedRewardWidget->SetImageUI(RewardOptionEntryAccepted.RewardOptionWidget->GetImage());
	//Attach the Accepted Reward Widget to its description widget.
	DescSource->GetContentPanel()->AddChild(AcceptedRewardWidget);
}

void UUDRoundRewardMenu::SetRewardOptionsEnabled(bool IsEnabled)
{
	//Enable/Disable each of the reward widget's "buttons", thus setting whether or not the user may select them.
	for (int i = 0; i < RewardOptionEntries.Num(); i++)
	{
		RewardOptionEntries[i].RewardOptionWidget->GetRewardButton()->SetIsEnabled(IsEnabled);
	}
}

void UUDRoundRewardMenu::InitDescriptionReceiver(TScriptInterface<IUDUIDescriptionReceiver> DescReceiver)
{
	DescriptionReceiver = DescReceiver;

	//Set the description reciever for the reward options.
	for (int i = 0; i < RewardOptionEntries.Num(); i++)
	{
		UUDDescriptionSourceWidget* DescSource = RewardOptionEntries[i].DescriptionSource;

		DescSource->DescriptionReceiver = DescReceiver;
	}
}

int32 UUDRoundRewardMenu::GetRewardSelected()
{
	return RewardIndexSelected;
}

bool UUDRoundRewardMenu::RewardIsSelected()
{
	return RewardIndexSelected >= 0;
}

void UUDRoundRewardMenu::SetRewardOptions(TArray<FRewardInfo> Rewards)
{
	//If a reward was previously selected, deselect it.
	if (RewardIsSelected())
	{
		RewardOptionEntries[RewardIndexSelected].RewardOptionWidget->OnRewardDeselected();
	}
	//Reset the reward selected
	RewardIndexSelected = -1;

	SelectedRewardText->SetText(FText::GetEmpty());

	//We retain the index between the two below loops
	int32 i = 0;

	//If there are rewards to be displayed, iterate through them and display them to the reward option entries. Otherwise, tell the player that there are no rewards!
	if (Rewards.Num() > 0)
	{
		//Display a message prompting the player to select a reward.
		PromptText->SetText(SelectionPromptMessage);

		while (i < Rewards.Num() && i < MaxRewardOptions)
		{
			//Get the Reward Option Entry that we will be setting.
			FRewardOptionEntry RewardEntry = RewardOptionEntries[i];

			//Get the data for displaying the reward.
			FRewardInfo RewardInfo = Rewards[i];

			//Make sure the Option Widget is visible.
			RewardEntry.RewardOptionWidget->SetVisibility(ESlateVisibility::Visible);

			//Set the reward's image;
			RewardEntry.RewardOptionWidget->SetImageUI(RewardInfo.RewardImage);

			//Set the reward's description.
			RewardEntry.DescriptionSource->SetDescription(RewardInfo.RewardDescription);

			i++;
		}
	}
	else
	{
		PromptText->SetText(NoRewardsMessage);
	}

	//Hide any remaining Reward Option Widgets if there weren't enough rewards to fill them all.
	while (i < RewardOptionEntries.Num())
	{
		//Get the Reward Option Entry that we will be setting.
		FRewardOptionEntry RewardEntry = RewardOptionEntries[i];

		RewardEntry.RewardOptionWidget->SetVisibility(ESlateVisibility::Collapsed);

		i++;
	}

	//Make sure the reward options are "enabled" so that the player can actually select them.
	SetRewardOptionsEnabled(true);
}

void UUDRoundRewardMenu::UpdateRewardOptionSelected(UUDRoundRewardOptionWidget* RewardOption)
{
	//Deselect any previously selected reward. If no reward was previously selected, enable the confirm button so the user can lock in their reward.
	if (RewardIsSelected())
	{
		RewardOptionEntries[RewardIndexSelected].RewardOptionWidget->OnRewardDeselected();
	}
	else
	{
		//Enable the confirm button.
		ConfirmRewardButton->SetIsEnabled(true);
	}

	int32 RewardIndex = RewardOption->GetRewardIndex();

	//Set the reward selected.
	RewardIndexSelected = RewardIndex;

	//Show the name of the selected reward to the user.
	FText RewardTitle = RewardOptionEntries[RewardIndex].DescriptionSource->GetDescription().Title;
	SelectedRewardText->SetText(RewardTitle);
}

UButton* const UUDRoundRewardMenu::GetConfirmButton()
{
	return ConfirmRewardButton;
}

int32 UUDRoundRewardMenu::GetMaxRewardOptions()
{
	return MaxRewardOptions; 
}

#undef LOCTEXT_NAMESPACE