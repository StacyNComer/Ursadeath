// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDUIDescriptionReceiver.h"
#include "UDRoundRewardMenu.generated.h"

class UTextBlock;
class UButton;
class UUDRoundRewardIconWidget;
class UUDRoundRewardOptionWidget;
class UUDDescriptionSourceWidget;
class IUDUIDescriptionReceiver;

USTRUCT(BlueprintType)
struct FRewardInfo
{
	GENERATED_BODY()

	/** The image the reward should be displayed as.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* RewardImage;

	/** The description of the reward.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FUIDescription RewardDescription;
};

USTRUCT(BlueprintType)
struct FRewardOptionEntry
{
	GENERATED_BODY()

	/** The description source that the reward option widget has as its parent.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UUDDescriptionSourceWidget> DescriptionSource;

	/** The reward option widget proper.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UUDRoundRewardOptionWidget> RewardOptionWidget;
};

/**
 * 
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundRewardMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The class that this menu's description sources will be created as.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDDescriptionSourceWidget> DescriptionSourceClass;

	/** The class the Round Reward Option Widgets will be created as.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDRoundRewardOptionWidget> RewardOptionWidgetClass;

	/** The class the Accepted Round Reward Widgets will be created as.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDRoundRewardIconWidget> RewardAcceptedWidgetClass;

	/** The UI panel in which the reward option widgets are created.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> RewardOptionPanel;

	/** The UI panel in which the accepted reward widgets are created.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> RewardsAcceptedPanel;

	/** A text block urging the player to choose their reward.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> RewardTitleText;

	/** A text block urging the player to choose their reward.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> PromptText;

	/** A text block showing the title of the current reward that the player has selected.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> SelectedRewardText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> ConfirmRewardButton;

	/** The maximums different rewards this widget will accept as an option.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 MaxRewardOptions = 3;

	/** The text telling the player what kind of reward the menu offers. Shown above the accepted rewards*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText RewardTitleMessage;

	/** The text that should appear when the player has rewards to choose from.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText SelectionPromptMessage;

	/** Text that appears when the player has selected and confirmed a reward.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText RewardAcceptedMessage;

	/** Text that appears when the their are no rewards being offered!*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText NoRewardsMessage;

	/** The description reciever that the reward icons/options will send their descriptions to.*/
	TScriptInterface<IUDUIDescriptionReceiver> DescriptionReceiver;

	TArray<FRewardOptionEntry> RewardOptionEntries;

	/** Which reward option the player has selected, corresponding with the index of the reward in RewardOptionEntries. This value is -1 while no reward is selected.*/
	int32 RewardIndexSelected = -1;

private:
	/** Locks in whatever reward the player has chosen. Meant to be bound to the Confirm Reward Button.*/
	UFUNCTION()
		void ConfirmReward();

protected:
	virtual void NativeOnInitialized() override;

	/** Sets whether or not the reward options can be clicked by the player. Note that this niether affects the options' visibility nor the player's ability to see their descriptions.*/
	void SetRewardOptionsEnabled(bool IsEnabled);

public:
	/** Sets the description receiver that gets sent descriptions from the reward icons and options. 
	*
	* Note that this function is meant strictly for initialization and will only update the reward options and not newly created reward icons.*/
	void InitDescriptionReceiver(TScriptInterface<IUDUIDescriptionReceiver> DescReceiver);

	/** Returns the index of the reward option that the player has selected. Returns -1 if no reward has been selected.*/
	int32 GetRewardSelected();

	/** Returns true if any of the reward options are selected.*/
	bool RewardIsSelected();

	/** Sets the currently available rewards options for the player to choose from. The reward options are also enabled so that the player can select one, but only if at least one reward option was added.*/
	void SetRewardOptions(TArray<FRewardInfo> RewardInfo);

	/** Updates the menu to account for the given reward option being selected. This includes calling the deselection method of any previously selected rewards option. 
	*
	* Note that it is assumed that the selection method for the given reward option was already called.*/
	void UpdateRewardOptionSelected(UUDRoundRewardOptionWidget* RewardOption);

	/** Returns the button that confirms the player's reward choice when pressed.*/
	UButton* const GetConfirmButton();
};
