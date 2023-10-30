// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDWaveEntryWidget.generated.h"

class UUDEnemySpawnIndicator;
class UUrsadeathGameInstance;
struct FEnemyWave;

/**
 * A Widget for showing the enemies the player will face in a wave from within the Upgrade Menu
 */
UCLASS()
class URSADEATH_API UUDWaveEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDEnemySpawnIndicator> WaveSpawnIndicatorClass;

	/** The widget used to contain the Enemy Spawn Indicators.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UPanelWidget> SpawnIndicatorContainer;

	/** A reference to the game instance. Makes it easy to get the spawning data for a particular enemy.*/
	TObjectPtr<UUrsadeathGameInstance> UrsadeathGameInstance;

	/** The indicator used to show the number of squire tier enemies that will be in a wave.*/
	TObjectPtr<UUDEnemySpawnIndicator> SquireSpawnIndicator;

	/** An array of spawn indicators used for Knight and Champion tier enemies.*/
	TArray<TObjectPtr<UUDEnemySpawnIndicator>> NonSquireSpawnIndicators;

protected:
	virtual void NativeOnInitialized() override;

	/** Create a spawn indicator and attaches it to the Spawn Indicator Container.*/
	UUDEnemySpawnIndicator* CreateSpawnIndicatorWidget();

public:
	/** Override to define what occurs when the wave recieves the number it will be labelled as.*/
	UFUNCTION(BlueprintImplementableEvent)
		void SetWaveNumber(int WaveNum);
	
	/** Sets the widget to show the contents of the given wave.*/
	UFUNCTION()
		void DisplayWave(FEnemyWave Wave);


	
};
