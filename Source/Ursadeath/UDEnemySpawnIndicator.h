// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDEnemySpawnIndicator.generated.h"

/**
 * A widget for displaying the number of an enemy type in a wave to the player. Rather than hold the enemy type itself, the widget takes an image meant to represent the enemy type.
 */
UCLASS(Abstract)
class URSADEATH_API UUDEnemySpawnIndicator : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The current number the indicator is displaying.*/
	int EnemyCount = 0;

public:
	/** Sets the widget to display a particular count value.*/
	void SetEnemyCount(int Count);

	/** Decrements the enemy count by one and displays the new value.*/
	void DecrementEnemyCount();
	
	/** Sets the indicator's image. This must be overriden in a blueprint to decide what is done with the given image.*/
	UFUNCTION(BlueprintImplementableEvent)
		void SetEnemyImage(UTexture2D* Image);

	/** Sets the image for the enemy's upgrade.*/
	UFUNCTION(BlueprintImplementableEvent)
		void SetUpgradeImage(UTexture2D* Image);

protected:
	virtual void NativeOnInitialized() override;

	/** Updates the widget to show the player the given count. Must be overriden in a blueprint to decide what is actual done with the given count.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnEnemyCountChanged(int Count);

};
