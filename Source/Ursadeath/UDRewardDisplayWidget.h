// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDRewardDisplayWidget.generated.h"

class UTextBlock;
class UUDDescriptionSourceWidget;
class UUDRoundRewardIconWidget;
class IUDUIDescriptionReceiver;

/**
 * A widget meant to hold rewards and their descriptions in a widget panel of some sort.
 */
UCLASS()
class URSADEATH_API UUDRewardDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** A text block urging the player to choose their reward.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> RewardTitleText;
	
	/** The UI panel in which the reward option widgets are created.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UPanelWidget> RewardDisplayPanel;

	/** The class that this menu's description sources will be created as.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUDDescriptionSourceWidget> DescriptionSourceClass;

	/** The text telling the player what kind of reward the menu offers. Shown above the accepted rewards*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RewardMessages)
		FText RewardTitleMessage;

	/** The description reciever that the reward icons will send their descriptions to.*/
	TScriptInterface<IUDUIDescriptionReceiver> DescriptionReceiver;

protected:
	virtual void NativeOnInitialized();

public:
	void InitDescriptionReceiver(TScriptInterface<IUDUIDescriptionReceiver> DescReceiver);

	void AddReward(UUDDescriptionSourceWidget* DescriptionSource, UUDRoundRewardIconWidget* RewardIconWidget);
};
