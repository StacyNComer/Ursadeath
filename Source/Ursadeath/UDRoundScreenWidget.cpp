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

