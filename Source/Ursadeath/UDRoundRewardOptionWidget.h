// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDRoundRewardIconWidget.h"
#include "UDRoundRewardOptionWidget.generated.h"

class UButton;
class UUDRoundRewardMenu;

/**
 * 
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundRewardOptionWidget : public UUDRoundRewardIconWidget
{
	GENERATED_BODY()

protected:
	/** The button used to select this reward.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> RewardButton;

	/** The menu this reward is being contained within. This menu is signalled whenever the reward is selected*/
	TObjectPtr<UUDRoundRewardMenu> ContainingMenu;

	/** The index in the containing menu's reward options this widget is stored at.*/
	int32 RewardEntryIndex;

protected:
	virtual void NativeOnInitialized() override;

	/** Calls OnRewardSelected and tells the owning menu that this widget has been selected.*/
	UFUNCTION()
		void NativeOnRewardSelected();

public:
	/** Override to define what should occur when this reward option is selected.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnRewardSelected();

	/** Override to define what should occur when something causes this reward to be deselected.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnRewardDeselected();

	/** Returns the button used to select this reward.*/
	UFUNCTION(BlueprintCallable)
		UButton* const GetRewardButton();

	/** Returns the index in the containing menu's reward options this widget is stored at.*/
	int32 GetRewardIndex();

	/** Sets the reward menu meant to contain this widget.*/
	void SetContainingMenu(UUDRoundRewardMenu* Menu, int32 RewardIndex);
};
