// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UDRoundRewardMenu.h"
#include "UDPlayerUpgrade.generated.h"

class AUDPlayerCharacter;

/**
 * The base class for upgrades that can be applied to the player.
 */
UCLASS(abstract, Blueprintable)
class URSADEATH_API UUDPlayerUpgrade : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerCharacter> OwningPlayer;

	/** Data for displaying the Upgrade to the player's UI.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FRewardInfo RewardInfo;

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void OnUpgradeAdded();

public:
	UFUNCTION(BlueprintCallable)
		static UUDPlayerUpgrade* CreatePlayerUpgrade(TSubclassOf<UUDPlayerUpgrade> UpgradeClass, AUDPlayerCharacter* Player);

	FRewardInfo GetRewardInfo();
};
