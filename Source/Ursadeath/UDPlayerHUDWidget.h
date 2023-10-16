// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "UDPlayerHUDWidget.generated.h"

class AUDEnemy;
class UHorizontalBox;
class UUDEnemySpawnIndicator;
class UDataTable;
struct FEnemyWave;
enum class EEnemyTier;

/** Holds information about spawning a given type of Knight tier enemy. Ony one entry should exist on the data table per class of enemy.*/
USTRUCT(BlueprintType)
struct FKnightSpawningData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AUDEnemy> KnightClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UTexture2D> EnemyIcon;
};

/**
 * 
 */
UCLASS(Abstract)
class URSADEATH_API UUDPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The widget used to contain the enemy indicators. The indicators are created and set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UPanelWidget> SpawnIndicatorContainer;
	
	/** The blueprint class that will be used for enemy spawn indicators*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDEnemySpawnIndicator> EnemySpawnIndicatorClass;

	/** A data table of all of the game's Knight tier enemies and any other information for spawning them. Cached from the game instance upon initialization.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<UDataTable> KnightSpawnDataTable;

	/** An indicator showing how many Squire tier enemies are left in a wave. All enemies types in the Squire tier share a single indicator.*/
	UUDEnemySpawnIndicator* SquireSpawnIndicator;

	/** A map linking the spawn indicators to the Knight tier enemy they represent. Squire tier enemies are all grouped into one indicator.*/
	TMap<TSubclassOf<AUDEnemy>, UUDEnemySpawnIndicator*> KnightSpawnIndicators;

private:
	/** Constructs a spawn indicator widget, sets the SpawnIndicatorContainer as its parent and sets its icon to the give texture.*/
	UUDEnemySpawnIndicator* CreateSpawnIndicatorWidget(TObjectPtr<UTexture2D> IndicatorIcon);

protected:
	virtual void NativeOnInitialized() override;

public:
	/**Called by the player character whenever their health changes. Override to define how the HUD should change in response to the player's health.*/
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateHealth(int PlayerHealth);

	/**Called by the player character whenever their energy changes. Override to define how the HUD should change in response to the player's energy.*/
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateEnergy(float PlayerEnergy);

	/** Decrement the count of the indicator for the given enemy type.*/
	void DecrementEnemyCount(TSubclassOf<AUDEnemy> EnemyClass, EEnemyTier EnemyTier);

	void DisplayEnemyWave(FEnemyWave Wave);
};
