// Fill out your copyright notice in the Description page of Project Settings.


#include "UDWaveEntryWidget.h"
#include "UDEnemySpawnIndicator.h"
#include "Blueprint/WidgetTree.h"
#include "UrsadeathGameInstance.h"
#include "UDDescriptionSourceWidget.h"
#include "UDEnemy.h"

void UUDWaveEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UrsadeathGameInstance = Cast<UUrsadeathGameInstance>(GetGameInstance());
	
	//Create the spawn indicator for Squires and set its image and description.
	SquireEnemyEntry = CreateEnemyEntry();
	SquireEnemyEntry->SpawnCounterWidget->SetEnemyImage(UrsadeathGameInstance->GetSquireSpawnData().EnemyIcon);
	SquireEnemyEntry->DescriptionSourceWidget->SetDescription(UrsadeathGameInstance->GetSquireSpawnData().Description);

	//Create spawn indicators for the non-Squire enemies. Instead of creating an indicator for each class of enemy, enough indicators are created for the Wave Entry to display the game instance's MaxNonSquireTypesPerWave.
	for(int i = 0; i < UrsadeathGameInstance->MaxNonSquireTypesPerWave; i++)
	{
		NonSquireEnemyEntries.Add(CreateEnemyEntry());
	}
}

void UUDWaveEntryWidget::DisplayWave(FEnemyWave Wave)
{
	//Display the number of squires in the wave
	SquireEnemyEntry->SpawnCounterWidget->SetEnemyCount(Wave.WaveData.SquireCount);

	//Generate an array of the enemy types to be iterated through
	TArray <TSubclassOf<AUDEnemy>> NonSquireClasses;
	Wave.KnightCounts.GenerateKeyArray(NonSquireClasses);

	//"i" is declared here since we'll need it for multiple loops.
	int i = 0;

	//Start be iterating through the enemy types, setting a Spawn Indicator to display enemy type's icon and the number present in the wave.
	while(i < NonSquireClasses.Num())
	{
		EnemyEntry* EnemyEntry = NonSquireEnemyEntries[i];
		UUDEnemySpawnIndicator* SpawnIndicator = EnemyEntry->SpawnCounterWidget;

		TSubclassOf<AUDEnemy> EnemyClass = NonSquireClasses[i];
		
		FEnemySpawnData SpawnDataEntry = UrsadeathGameInstance->GetSpawnDataEntry(EnemyClass);

		SpawnIndicator->SetEnemyImage(SpawnDataEntry.EnemyIcon);

		SpawnIndicator->SetEnemyCount(Wave.KnightCounts[EnemyClass]);

		EnemyEntry->DescriptionSourceWidget->SetDescription(SpawnDataEntry.Description);

		//Make sure the spawn indicator is visible.
		SpawnIndicator->SetVisibility(ESlateVisibility::Visible);

		i++;
	}

	//Then, if there are less enemy types than Spawn Indicators, hide the extra spawn indicators.
	//Because "i" is tracked outside of the loops, we can pick up where the previous loop left off.
	while (i < NonSquireEnemyEntries.Num())
	{
		NonSquireEnemyEntries[i]->SpawnCounterWidget->SetVisibility(ESlateVisibility::Collapsed);

		i++;
	}
}

EnemyEntry* UUDWaveEntryWidget::CreateEnemyEntry()
{
	//Contruct the Description Source Widget and bind it to the pointer address given to this method.
	UUDDescriptionSourceWidget* DescriptionSource = WidgetTree->ConstructWidget<UUDDescriptionSourceWidget>(DescriptionSourceWidgetClass);

	//Contruct the indicator widget.
	UUDEnemySpawnIndicator* SpawnIndicator = WidgetTree->ConstructWidget<UUDEnemySpawnIndicator>(WaveSpawnIndicatorClass);

	//Add the description source as a child of the widget panel meant to contain them.
	SpawnIndicatorContainer->AddChild(DescriptionSource);

	//Place the spawn counter within the description source.
	DescriptionSource->GetContentPanel()->AddChild(SpawnIndicator);

	return new EnemyEntry(SpawnIndicator, DescriptionSource);
}

void UUDWaveEntryWidget::SetDescriptionReceiver(TScriptInterface<IUDUIDescriptionReceiver> DescriptionReceiver)
{
	SquireEnemyEntry->DescriptionSourceWidget->DescriptionReceiver = DescriptionReceiver;

	for (int i = 0; i < NonSquireEnemyEntries.Num(); i++)
	{
		NonSquireEnemyEntries[i]->DescriptionSourceWidget->DescriptionReceiver = DescriptionReceiver;
	}
}

