// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDRoundScreenWidget.generated.h"

class UUDWaveEntryWidget;
class UUrsadeathGameInstance;
class UButton;
struct FEnemyWave;

/**
 * A widget used for the player's round menu, where they can see the upcoming waves in the game as well as their upgrades.
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundScreenWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	/** The widget acting as the container for the round's waves.*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UPanelWidget> WaveEntryContainer;

protected:
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUrsadeathGameInstance> UrsadeathGameInstance;

	/** The subclass used for the Round Display's wave entry.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RoundDisplay)
		TSubclassOf<UUDWaveEntryWidget> WaveEntryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoundDisplay, meta = (BindWidget))
		TObjectPtr<UButton> RoundStartButton;

	/** The wave entry widgets used to display the contents of each enemy wave within the round.*/
	TArray<TObjectPtr<UUDWaveEntryWidget>> WaveEntries;

private:
	UFUNCTION()
		void OnRoundStartPressed();

protected:
	/** Sets the UI to display the given Round Number.*/
	UFUNCTION(BlueprintImplementableEvent, Category = RoundDisplay)
		void SetRoundNumber(int RoundNumber);

	virtual void NativeOnInitialized() override;

public:
	/** Display the given round of enemy waves to this widget. This method assumes that the array has equal or less arrays than the game instance's "MaxWavesPerRound".*/
	UFUNCTION(BlueprintCallable, Category = RoundDisplay)
		void DisplayRound(int RoundNumber, TArray<FEnemyWave> RoundWaves);

	UButton* const GetRoundStartButton();

	/** Upgrades are not in the game, so currently this simply enables the Round Start button.*/
		void SetRoundRewards();
	
};
