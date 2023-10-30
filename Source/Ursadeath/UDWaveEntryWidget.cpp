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
	
	SquireSpawnIndicator = CreateSpawnIndicatorWidget();
	SquireSpawnIndicator->SetEnemyImage(UrsadeathGameInstance->SquireIcon);

	for(int i = 0; i < UrsadeathGameInstance->MaxKnightTypesPerWave; i++)
	{
		NonSquireSpawnIndicators.Add(CreateSpawnIndicatorWidget());
	}
}

void UUDWaveEntryWidget::DisplayWave(FEnemyWave Wave)
{
	SquireSpawnIndicator->SetEnemyCount(Wave.SquireSpawns);

	TArray <TSubclassOf<AUDEnemy>> NonSquireClasses;
	Wave.EnemyCounts.GenerateKeyArray(NonSquireClasses);

	for (int i = 0; i < NonSquireClasses.Num(); i++)
	{
		UUDEnemySpawnIndicator* SpawnIndicator = NonSquireSpawnIndicators[i];
		
		FEnemySpawnData SpawnDataEntry = UrsadeathGameInstance->GetSpawnDataEntry(NonSquireClasses[i]);

		SpawnIndicator->SetEnemyImage(SpawnDataEntry.EnemyIcon);
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

