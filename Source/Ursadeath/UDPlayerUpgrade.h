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
UCLASS(abstract, Blueprintable, BlueprintType)
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
	/** Called when the player selects this upgrade after it is attached to them.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnUpgradeAdded(AUDPlayerCharacter* Player);

	/** Attach the upgrade to the given player so that the Garbage Collection doesn't abduct it. This can be overriden in child classes incase the upgrade's relationship with the player is more complex.*/
	virtual void AttachToPlayer(AUDPlayerCharacter* Player);

public:
	/** A getter for the world context object so that Upgrades can use the Kismet Library in blueprints.*/
	virtual UWorld* GetWorld() const override;

	/** Creates an upgrade and applies it to the given player.*/
	UFUNCTION(BlueprintCallable)
		static UUDPlayerUpgrade* CreatePlayerUpgrade(TSubclassOf<UUDPlayerUpgrade> UpgradeClass, AUDPlayerCharacter* Player);
};
