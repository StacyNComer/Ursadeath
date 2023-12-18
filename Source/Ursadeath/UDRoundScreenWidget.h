// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDUIDescriptionReceiver.h"
#include "UDRoundScreenWidget.generated.h"

class UUDWaveEntryWidget;
class UUrsadeathGameInstance;
class UUDRoundRewardMenu;
class UButton;
class UTextBlock;
struct FEnemyWave;

/**
 * A widget used for the player's round menu, where they can see the upcoming waves in the game as well as their upgrades.
 */
UCLASS(Abstract)
class URSADEATH_API UUDRoundScreenWidget : public UUserWidget, public IUDUIDescriptionReceiver
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

	UPROPERTY(BlueprintReadWrite, Category = RoundDisplay, meta = (BindWidget))
		TObjectPtr<UButton> RoundStartButton;

	UPROPERTY(BlueprintReadWrite, Category = DescriptionUI, meta = (BindWidget))
		TObjectPtr<UTextBlock> DescriptionTitleUI;
	
	UPROPERTY(BlueprintReadWrite, Category = DescriptionUI, meta = (BindWidget))
		TObjectPtr<UTextBlock> DescriptionBodyUI;

	/** The description shown when a description source for this UI is no longer being moused over.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DescriptionUI)
		FUIDescription DefaultDescription;

	/** The "Reward" menu for viewing or adding to the Knight enemy types in play.*/
	UPROPERTY(BlueprintReadWrite, Category = RoundRewards, meta = (BindWidget))
		TObjectPtr<UUDRoundRewardMenu> KnightRewardMenu;

	UPROPERTY(BlueprintReadWrite, Category = RoundRewards, meta = (BindWidget))
		TObjectPtr<UUDRoundRewardMenu> UpgradeRewardMenu;

	/** The wave entry widgets used to display the contents of each enemy wave within the round.*/
	TArray<TObjectPtr<UUDWaveEntryWidget>> WaveEntries;

private:
	UFUNCTION()
		void OnRoundStartPressed();

	/** Sets the round's description text. Used by the Description Reciever interface.*/
	void SetDescriptionText(FUIDescription Description);

protected:
	/** Sets the UI to display the given Round Number.*/
	UFUNCTION(BlueprintImplementableEvent, Category = RoundDisplay)
		void SetRoundNumber(int RoundNumber);

	virtual void NativeOnInitialized() override;

public:
	/** Display the given round of enemy waves to this widget. This method assumes that the array has equal or less arrays than the game instance's "MaxWavesPerRound".*/
	UFUNCTION(BlueprintCallable, Category = RoundDisplay)
		void DisplayRound(int RoundNumber, TArray<FEnemyWave> RoundWaves);

	/** Allow the player to start the round by enabling the round button. Used for delegates.*/
	UFUNCTION()
		void EnableRoundStart();

	UButton* const GetRoundStartButton();

	/** Returns the "reward" menus for view or adding Knight enemy types in play.*/
	UUDRoundRewardMenu* const GetKnightRewardMenu();

	UUDRoundRewardMenu* const GetUpgradeRewardMenu();

	/** Upgrades are not in the game, so currently this simply enables the Round Start button.*/
	void SetRoundRewards();
	
	// Begin UIDescriptionReciever Interface
		/** Sets the title and body text for the Round Screen's description.*/
		void ReceiveDescription(FUIDescription Description) override;

		void SetDefaultDescription() override;
	// End UIDescriptionReciever Interface

};
