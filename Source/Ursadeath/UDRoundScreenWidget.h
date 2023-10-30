// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDRoundScreenWidget.generated.h"

class UUDWaveEntryWidget;

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

	TArray<TObjectPtr<UUDWaveEntryWidget>> WaveEntries;

protected:
	virtual void NativeOnInitialized() override;
	
};
