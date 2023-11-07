// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDRoundScreenWidget.generated.h"

class UUDWaveEntryWidget;
struct FEnemyWave;

/**
 * A widget used for the player's round menu, where they can see the upcoming waves in the game as well as their upgrades.
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The widget acting as the container for the round's waves.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = RoundDisplay)
		TObjectPtr<UPanelWidget> WaveEntryContainer;

	/** The subclass used for the Round Display's wave entry.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RoundDisplay)
		TSubclassOf<UUDWaveEntryWidget> WaveEntryClass;

	/** The wave entry widgets used to display the contents of each enemy wave within the round.*/
	TArray<TObjectPtr<UUDWaveEntryWidget>> WaveEntries;

protected:
	virtual void NativeOnInitialized() override;

public:
	/** Display the given round of enemy waves to this widget. This method assumes that the array has equal or less arrays than the game instance's "MaxWavesPerRound".*/
	UFUNCTION(BlueprintCallable)
		void DisplayRound(TArray<FEnemyWave> RoundWaves);
	
};
