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
class UUDRewardDisplayWidget;

/**
* A struct for holding the icon, title, and descrtiption of the rewards to be displayed in the menu. 
*/
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
 * The base class for menus for choosing rewards and displaying already chosen rewards.
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundRewardMenu : public UUserWidget
{
	GENERATED_BODY()

private:
	/** True if the menu has reward options to be chosen and the player has yet to choose one.*/
	bool bMenuActive;

	/** If true, the menu will continue to be active even after a reward is picked. Note that the reward will not automatically be repopulated.*/
	bool bRepeatReward;

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

	/** The widget meant to display what rewards the player has accepted.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UUDRewardDisplayWidget> RewardDisplayWidget;

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

	/** The text that should appear when the player has rewards to choose from.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText SelectionPromptMessage;

	/** Text that appears when the player has selected and confirmed a reward.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText RewardAcceptedMessage;

	/** Text that appears when the their are no rewards being offered!*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText NoRewardsMessage;

	/** Text that appears if a reward is locked or forbidden for any reason. Unlike, the other messages, this must be manually shown via the ShowLockedMessage function.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText RewardLockedMessage;

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

	/** Returns the naximum number of reward options that this widget is meant to have available at once.*/
	int32 GetMaxRewardOptions();

	/** Returns true if the menu has at least one reward option to choose from and the player has yet to choose one.*/
	bool GetMenuActive();

	void SetRepeatReward(bool bNewRepeatReward);

	/** Causes the Prompt Text Block to show the RewardLockedMessage as its text. This lasts until something else changes the text, such as the reward options being set.*/
	void ShowLockedMessage();
};
