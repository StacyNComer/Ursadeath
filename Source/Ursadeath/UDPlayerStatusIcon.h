// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDPlayerStatusIcon.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * A widget for displaying the players status effects or similar data (such as if an upgrade has a cooldown or "stacks")
 */
UCLASS(abstract)
class URSADEATH_API UUDPlayerStatusIcon : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> StatusBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> StatusCounter;

	float MaxStatusBarTime;

	float CurrentStatusBarTime;
	
public:
	/** If true, the status counter will display the remaining time until the status bar is completely empty.*/
	UPROPERTY(BlueprintReadWrite)
		bool bCounterDisplaysStatusTime = true;

	/** If true, this widget will become invisible when the CurrentStatusCount becomes less than or equal to CurrentStatusBarValue. The widget will reappear when this value is set to be above 0.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDisappearWhenStatusDepleted = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHideCounterWhenStatusDepleted = false;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/** Sets the status icon's bar to drain from full to empty over the given amount of time. It is also possible to use GetStatusBar to interact with the bar directly.*/
	UFUNCTION(BlueprintCallable)
	void StartStatusBarFromTime(float MaxStatusTime, float CurrentStatusTime);
	
	UFUNCTION(BlueprintCallable)
		void StartStatusBar(float StatusTime);

	/** Decrease the status bar's time. Positive values will increase it instead. Make sure you call StartStatusBar to make the icon visible and set its max value.*/
	UFUNCTION(BlueprintCallable)
		void DecrementStatusTime(float DeltaTime);

	/** Returns the status icon's progress bar.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		UProgressBar* const GetStatusBar();

	/** Returns the text block */
	UFUNCTION(BlueprintCallable, BlueprintPure)
		UTextBlock* const GetStatusCounter();

	/** Sets the image shown by the status icon widget,*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetStatusImage(UTexture2D* Image);
};
