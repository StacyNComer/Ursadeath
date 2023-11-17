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

/**
 * 
 */
UCLASS(Abstract)
class URSADEATH_API UUDPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The widget used to contain the Enemy Spawn Indicators.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UPanelWidget> SpawnIndicatorContainer;
	
	/** The blueprint class that will be used for enemy spawn indicators.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDEnemySpawnIndicator> EnemySpawnIndicatorClass;

	/** A data table of all of the game's Knight tier enemies and any other information for spawning them. Cached from the game instance upon initialization.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<UDataTable> KnightSpawnDataTable;

	/** Tracks the seconds until an announcement being shown is hidden.*/
	UPROPERTY(BlueprintReadOnly)
		float AnnouncementTimeTracker;

	/** An indicator showing how many Squire tier enemies are left in a wave. All enemies types in the Squire tier share a single indicator.*/
	UUDEnemySpawnIndicator* SquireSpawnIndicator;

	/** A map linking the spawn indicators to the Knight tier enemy they represent. Squire tier enemies are all grouped into one indicator.*/
	TMap<TSubclassOf<AUDEnemy>, UUDEnemySpawnIndicator*> KnightSpawnIndicators;

private:
	/** Constructs a spawn indicator widget, sets the SpawnIndicatorContainer as its parent and sets its icon to the give texture.*/
	UUDEnemySpawnIndicator* CreateSpawnIndicatorWidget(TObjectPtr<UTexture2D> IndicatorIcon);

protected:
	/** Called when an announcement's visibility should be changed to the given value.*/
	UFUNCTION(BlueprintImplementableEvent)
		void SetAnnouncementVisibility(ESlateVisibility Visibilty);

	/** Called when the announcement text is changed.*/
	UFUNCTION(BlueprintImplementableEvent)
		void SetAnnounementText(const FText& Message);

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/**Called by the player character whenever their health changes. When Health Change is 0, the player's health is being initialized at the start of the game. Override to define how the HUD should change in response to the player's health.*/
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateHealth(int PlayerHealth, int HealthChange);

	/**Called by the player character whenever their energy changes. When the Energy Change is 0, the player's energy is being initialized at the start of the game. Override to define how the HUD should change in response to the player's energy.*/
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateEnergy(float PlayerEnergy, float EnergyChange, bool EnergyBarGained);

	/** Displays the given text to the player as an announcement for the given amount of time. Note that this will overrite any previously displayed announcement as well as its DisplayTime.*/
	void DisplayAnnouncement(const FText& Message, float DisplayTime);

	/** Decrement the count of the indicator for the given enemy type.*/
	void DecrementEnemyCount(TSubclassOf<AUDEnemy> EnemyClass, EEnemyTier EnemyTier);

	void DisplayEnemyWave(FEnemyWave Wave);
};
