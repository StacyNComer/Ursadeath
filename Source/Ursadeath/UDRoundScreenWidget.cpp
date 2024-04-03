// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundScreenWidget.h"
#include "UrsadeathGameInstance.h"
#include "UDRoundRewardMenu.h"
#include "Blueprint/WidgetTree.h"
#include "UDWaveEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "UDPlayerCharacter.h"

#define LOCTEXT_NAMESPACE "PlayerHUD"

void UUDRoundScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OwningPlayer = GetOwningPlayerPawn<AUDPlayerCharacter>();

	SetIsFocusable(true);

	UrsadeathGameInstance = GetGameInstance<UUrsadeathGameInstance>();

	KnightRewardMenu->InitDescriptionReceiver(this);

	UpgradeRewardMenu->InitDescriptionReceiver(this);

	//Create the Wave Entry widgets.
	for (int i = 0; i < UrsadeathGameInstance->MaxWavesPerRound; i++)
	{
		UUDWaveEntryWidget* WaveEntryWidget = WidgetTree->ConstructWidget<UUDWaveEntryWidget>(WaveEntryClass);

		WaveEntryWidget->SetDescriptionReceiver(this);

		WaveEntries.Add(WaveEntryWidget);

		//Attach the entry widget to its containter.
		WaveEntryContainer->AddChild(WaveEntryWidget);
		
		//Give the wave entry its wave number.
		WaveEntryWidget->SetWaveNumber(i+1);
	}

	//Bind the round Start Button.
	RoundStartButton->OnClicked.AddDynamic(this, &UUDRoundScreenWidget::TryRoundStart);

	//Bind the round start confirmation button.
	ConfirmStartButton->OnClicked.AddDynamic(this, &UUDRoundScreenWidget::ConfirmRoundStart);

	//Bind the Start COnfirmation Menu's back button
	ConfirmBackButton->OnClicked.AddDynamic(this, &UUDRoundScreenWidget::CloseStartConfirmationMenu);

	//Bind the confirm button for knight rewards so that it adds the chosen enemy type.
	KnightRewardMenu->GetConfirmButton()->OnClicked.AddDynamic(UrsadeathGameInstance, &UUrsadeathGameInstance::AddKnightReward);

	//Bind the confirm button for the Upgrade Rewards so that it gives the player their chosen upgrade.
	UpgradeRewardMenu->GetConfirmButton()->OnClicked.AddDynamic(UrsadeathGameInstance, &UUrsadeathGameInstance::AddUpgradeReward);

	//Allow the player to start the round after they choose the round's new knight type.
	KnightRewardMenu->GetConfirmButton()->OnClicked.AddDynamic(this, &UUDRoundScreenWidget::EnableRoundStart);
}

bool UUDRoundScreenWidget::GetHideConfirmationMenu()
{
	//If the hide confirmation checkbox was checked, the confirmation menu should not be shown until the game is reset.
	return HideConfirmationCheckbox->IsChecked();
}

void UUDRoundScreenWidget::ConfirmRoundStart()
{
	RoundStartButton->SetIsEnabled(false);

	OwningPlayer->ToggleRoundMenu();

	StartConfirmationWidget->SetVisibility(ESlateVisibility::Collapsed);
	
	StartButtonText->SetText(LOCTEXT("StartButtonRoundInProgress", "Round In Progress"));

	UrsadeathGameInstance->StartRound();
}

void UUDRoundScreenWidget::TryRoundStart()
{
	if (!GetHideConfirmationMenu() && UpgradeRewardMenu->GetMenuActive())
	{
		StartConfirmationWidget->SetVisibility(ESlateVisibility::Visible);
		
		return;
	}

	ConfirmRoundStart();
}

void UUDRoundScreenWidget::CloseStartConfirmationMenu()
{
	StartConfirmationWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UUDRoundScreenWidget::SetDescriptionText(FUIDescription Description)
{
	DescriptionTitleUI->SetText(Description.Title);

	DescriptionBodyUI->SetText(Description.Body);
}

void UUDRoundScreenWidget::DisplayRound(int RoundNumber, TArray<FEnemyWave> RoundWaves)
{
	SetRoundNumber(RoundNumber);

	//"i" is declared here since we'll need it for multiple loops.
	int i = 0;

	//First, Iterate through the Round Waves, setting the corresponding index of Wave Entry Widget to display it.
	while (i < RoundWaves.Num())
	{
		UUDWaveEntryWidget* WaveEntry = WaveEntries[i];

		//Set the wave entry to display its wave.
		WaveEntry->DisplayWave(RoundWaves[i]);

		//Make sure the wave entry is visible.
		WaveEntry->SetVisibility(ESlateVisibility::Visible);

		i++;
	}

	//Then, if there were less waves in the round than wave entry widgets, hide the extra wave entries.
	//Because "i" is tracked outside of the loops, we can pick up where the previous loop left off.
	while (i < WaveEntries.Num())
	{
		WaveEntries[i]->SetVisibility(ESlateVisibility::Collapsed);

		i++;
	}
}

void UUDRoundScreenWidget::EnableRoundStart()
{
	RoundStartButton->SetIsEnabled(true);

	StartButtonText->SetText(LOCTEXT("StartButtonCanStartRound", "Start Round"));
}

UButton* const UUDRoundScreenWidget::GetRoundStartButton()
{
	return RoundStartButton;
}

UTextBlock* const UUDRoundScreenWidget::GetStartButtonText()
{
	return StartButtonText;
}

UUDRoundRewardMenu* const UUDRoundScreenWidget::GetKnightRewardMenu()
{
	return KnightRewardMenu;
}

UUDRoundRewardMenu* const UUDRoundScreenWidget::GetUpgradeRewardMenu()
{
	return UpgradeRewardMenu;
}

void UUDRoundScreenWidget::ReceiveDescription(FUIDescription Description)
{
	SetDescriptionText(Description);
}

void UUDRoundScreenWidget::SetDefaultDescription()
{
	SetDescriptionText(DefaultDescription);
}


#undef LOCTEXT_NAMESPACE
