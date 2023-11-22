// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDWaveEntryWidget.generated.h"

class UUDEnemySpawnIndicator;
class UUDDescriptionSourceWidget;
class IUDUIDescriptionReceiver;
class UUrsadeathGameInstance;
struct FEnemyWave;

/** A struct acting as a container for an Enemy Counter and the Description Source describing the enemy.*/
struct EnemyEntry
{
	EnemyEntry(TObjectPtr<UUDEnemySpawnIndicator> SpawnCounterWidget, TObjectPtr<UUDDescriptionSourceWidget> DescriptionSourceWidget)
	{
		this->SpawnCounterWidget = SpawnCounterWidget;

		this->DescriptionSourceWidget = DescriptionSourceWidget;
	}

	TObjectPtr<UUDEnemySpawnIndicator> SpawnCounterWidget;

	TObjectPtr<UUDDescriptionSourceWidget> DescriptionSourceWidget;
};

/**
 * A Widget for showing the enemies the player will face in a wave from within the Round Screen
 */
UCLASS()
class URSADEATH_API UUDWaveEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The class of Description Source which will act as the wrapper for each Spawn Indicator. This is what causes the spawn indicators to display a description.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf <UUDDescriptionSourceWidget> DescriptionSourceWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDEnemySpawnIndicator> WaveSpawnIndicatorClass;

	/** The widget used to contain the Enemy Spawn Indicators.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UPanelWidget> SpawnIndicatorContainer;

	/** A reference to the game instance. Makes it easy to get the spawning data for a particular enemy.*/
	TObjectPtr<UUrsadeathGameInstance> UrsadeathGameInstance;

	/** The indicator used to show the number of squire tier enemies that will be in a wave.*/
	EnemyEntry* SquireEnemyEntry;

	/** An array of spawn indicators used for Knight and Champion tier enemies.*/
	TArray<EnemyEntry*> NonSquireEnemyEntries;

protected:
	virtual void NativeOnInitialized() override;

	/** Creates and returns an Enemy Entry meant to hold an Enemy Counter and Description Source for this widget.*/
	EnemyEntry* CreateEnemyEntry();

public:
	/** Override to define what occurs when the wave recieves the number it will be labelled as.*/
	UFUNCTION(BlueprintImplementableEvent)
		void SetWaveNumber(int WaveNum);
	
	/** Sets the widget to show the contents of the given wave. This method assumes that the number of Non-Squire enemy types does not exceed the game instance's */
	UFUNCTION()
		void DisplayWave(FEnemyWave Wave);

	/** Sets the description receiver for all of the spawn counter's description sources.*/
	void SetDescriptionReceiver(TScriptInterface<IUDUIDescriptionReceiver> DescriptionReceiver);
};
