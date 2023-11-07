// Fill out your copyright notice in the Description page of Project Settings.


#include "UDRoundScreenWidget.h"
#include "UrsadeathGameInstance.h"
#include "Blueprint/WidgetTree.h"
#include "UDWaveEntryWidget.h"

void UUDRoundScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UUrsadeathGameInstance* UrsadeathGameInstance = Cast<UUrsadeathGameInstance>(GetGameInstance());

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
}

void UUDRoundScreenWidget::DisplayRound(TArray<FEnemyWave> RoundWaves)
{
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

