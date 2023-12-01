// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDGameOverWidget.generated.h"

class UTextBlock;

/**
 * The UI which should appear when the player completes (or loses) the game.
 */
UCLASS(Abstract)
class URSADEATH_API UUDGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> RoundCounter;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> WaveCounter;

public:
	/** Set the UI to show the players accomplishments in their run.*/
	void DisplayScore(int RoundsCompleted, int WavesCompleted);
	
};
