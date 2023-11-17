// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundScreenWidget.h"
#include "UrsadeathGameInstance.h"
#include "Blueprint/WidgetTree.h"
#include "UDWaveEntryWidget.h"
#include "Components/Button.h"

void UUDRoundScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(true);

	UrsadeathGameInstance = GetGameInstance<UUrsadeathGameInstance>();

	//Create the Wave Entry widgets.
	for (int i = 0; i < UrsadeathGameInstance->MaxWavesPerRound; i++)
	{
		UUDWaveEntryWidget* WaveEntryWidget = WidgetTree->ConstructWidget<UUDWaveEntryWidget>(WaveEntryClass);

		WaveEntries.Add(WaveEntryWidget);

		//Attach the entry widget to its containter.
		WaveEntryContainer->AddChild(WaveEntryWidget);
		
		//Give the wave entry its wave number.
		WaveEntryWidget->SetWaveNumber(i+1);
	}

	RoundStartButton->OnClicked.AddDynamic(this, &UUDRoundScreenWidget::OnRoundStartPressed);
}

void UUDRoundScreenWidget::OnRoundStartPressed()
{
	RoundStartButton->SetIsEnabled(false);
	
	UrsadeathGameInstance->StartRound();
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

void UUDRoundScreenWidget::SetRoundRewards()
{
	RoundStartButton->SetIsEnabled(true);
}

UButton* const UUDRoundScreenWidget::GetRoundStartButton()
{
	return RoundStartButton;
}

