// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerHUDWidget.h"
#include "Engine/DataTable.h"
#include "UDEnemySpawnIndicator.h"
#include "Blueprint/WidgetTree.h"
#include "UDEnemy.h"
#include "UDArena.h"
#include "UrsadeathGameInstance.h"

void UUDPlayerHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UUrsadeathGameInstance* UrsadeathGameInstance = Cast<UUrsadeathGameInstance>(GetGameInstance());

	//Get the spawning data from the game instance.
	KnightSpawnDataTable = UrsadeathGameInstance->KnightSpawnDataTable;

	//Create the indicator widget for squires.
	SquireSpawnIndicator = CreateSpawnIndicatorWidget(UrsadeathGameInstance->SquireIcon);

	//Get an array of all game's types of Knight tier enemies from the data table.
	TArray<FKnightSpawningData*> KnightSpawnData;
	KnightSpawnDataTable->GetAllRows<FKnightSpawningData>(TEXT("PlayerHUDKnightIndicatorInit"), KnightSpawnData);

	//Create an indicator for each type of Knight tier enemy 
	for (int i = 0; i < KnightSpawnData.Num(); i++)
	{
		const FKnightSpawningData* KnightSpawnDataEntry = KnightSpawnData[i];

		//Contruct the indicator widget.
		UUDEnemySpawnIndicator* KnightSpawnIndicator = CreateSpawnIndicatorWidget(KnightSpawnDataEntry->EnemyIcon);

		//Add the indicator to the Knight Spawn Indicator map so that it is linked to its enemy type.
		KnightSpawnIndicators.Add(KnightSpawnData[i]->KnightClass, KnightSpawnIndicator);
	}
}

UUDEnemySpawnIndicator* UUDPlayerHUDWidget::CreateSpawnIndicatorWidget(TObjectPtr<UTexture2D> IndicatorIcon)
{
	//Contruct the indicator widget.
	UUDEnemySpawnIndicator* SpawnIndicator = WidgetTree->ConstructWidget<UUDEnemySpawnIndicator>(EnemySpawnIndicatorClass);

	//Set the indicator's icon to that of the enemy.
	SpawnIndicator->SetEnemyImage(IndicatorIcon);

	//Add the indicator as a child of the widget panel meant to contain them.
	SpawnIndicatorContainer->AddChild(SpawnIndicator);

	return SpawnIndicator;
}

void UUDPlayerHUDWidget::DecrementEnemyCount(TSubclassOf<AUDEnemy> EnemyClass, EEnemyTier EnemyTier)
{
	switch (EnemyTier)
	{
		case EEnemyTier::SQUIRE:
		{
			SquireSpawnIndicator->DecrementEnemyCount();
			break;
		}

		case EEnemyTier::KNIGHT:
		{
			KnightSpawnIndicators[EnemyClass]->DecrementEnemyCount();
			break;
		}
	}
}

void UUDPlayerHUDWidget::DisplayEnemyWave(FEnemyWave Wave)
{
	SquireSpawnIndicator->SetEnemyCount(Wave.SquireSpawns);

	TArray<TSubclassOf<AUDEnemy>> KnightClasses;
	KnightSpawnIndicators.GenerateKeyArray(KnightClasses);

	for (int i = 0; i < KnightClasses.Num(); i++)
	{
		TSubclassOf<AUDEnemy> KnightClass = KnightClasses[i];
		UUDEnemySpawnIndicator* SpawnIndicator = KnightSpawnIndicators[KnightClass];

		if (Wave.EnemyCounts.Contains(KnightClass))
		{
			SpawnIndicator->SetEnemyCount(Wave.EnemyCounts[KnightClass]);
			SpawnIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			SpawnIndicator->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}