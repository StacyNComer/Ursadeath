// Fill out your copyright notice in the Description page of Project Settings.


#include "UDWaveEntryWidget.h"
#include "UDEnemySpawnIndicator.h"
#include "Blueprint/WidgetTree.h"
#include "UrsadeathGameInstance.h"
#include "UDEnemy.h"

void UUDWaveEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UrsadeathGameInstance = Cast<UUrsadeathGameInstance>(GetGameInstance());
	
	//Create the spawn indicator for Squires and set its image.
	SquireSpawnIndicator = CreateSpawnIndicatorWidget();
	SquireSpawnIndicator->SetEnemyImage(UrsadeathGameInstance->SquireIcon);

	//Create spawn indicators for the non-Squire enemies. Instead of creating an indicator for each class of enemy, enough indicators are created for the Wave Entry to display the game instance's MaxNonSquireTypesPerWave.
	for(int i = 0; i < UrsadeathGameInstance->MaxNonSquireTypesPerWave; i++)
	{
		NonSquireSpawnIndicators.Add(CreateSpawnIndicatorWidget());
	}
}

void UUDWaveEntryWidget::DisplayWave(FEnemyWave Wave)
{
	//Display the number of squires in the wave
	SquireSpawnIndicator->SetEnemyCount(Wave.WaveData.SquireCount);

	//Generate an array of the enemy types to be iterated through
	TArray <TSubclassOf<AUDEnemy>> NonSquireClasses;
	Wave.KnightCounts.GenerateKeyArray(NonSquireClasses);

	int i = 0;

	//Start be iterating through the enemy types, setting a Spawn Indicator to display enemy type's icon and the number present in the wave.
	while(i < NonSquireClasses.Num())
	{
		UUDEnemySpawnIndicator* SpawnIndicator = NonSquireSpawnIndicators[i];

		TSubclassOf<AUDEnemy> EnemyClass = NonSquireClasses[i];
		
		FEnemySpawnData SpawnDataEntry = UrsadeathGameInstance->GetSpawnDataEntry(EnemyClass);

		SpawnIndicator->SetEnemyImage(SpawnDataEntry.EnemyIcon);

		SpawnIndicator->SetEnemyCount(Wave.KnightCounts[EnemyClass]);

		//Make sure the spawn indicator is visible.
		SpawnIndicator->SetVisibility(ESlateVisibility::Visible);

		i++;
	}

	//Then, if there are less enemy types than Spawn Indicators, hide the extra spawn indicators.
	//Because "i" is tracked outside of the loops, we can pick up where the previous loop left off.
	while (i < NonSquireSpawnIndicators.Num())
	{
		NonSquireSpawnIndicators[i]->SetVisibility(ESlateVisibility::Collapsed);

		i++;
	}
}

UUDEnemySpawnIndicator* UUDWaveEntryWidget::CreateSpawnIndicatorWidget()
{
	//Contruct the indicator widget.
	UUDEnemySpawnIndicator* SpawnIndicator = WidgetTree->ConstructWidget<UUDEnemySpawnIndicator>(WaveSpawnIndicatorClass);

	//Add the indicator as a child of the widget panel meant to contain them.
	SpawnIndicatorContainer->AddChild(SpawnIndicator);

	return SpawnIndicator;
}

