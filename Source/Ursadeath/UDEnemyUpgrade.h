// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UDEnemyUpgrade.generated.h"

class AUDEnemy;

/**
 * 
 */
UCLASS(abstract, Blueprintable, BlueprintType)
class URSADEATH_API UUDEnemyUpgrade : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	/** Applies the upgrade to the given enemy. Upgrades may be applied to an idefinite amount of enemies.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnUpgrageApplied(AUDEnemy* Enemy);

	UFUNCTION(BlueprintCallable)
		/** Creates an uprade instance of the given type with the given UObject as its "Outer"*/
		static UUDEnemyUpgrade* CreateUpgrade(TSubclassOf<UUDEnemyUpgrade> UpgradeClass, UObject* Outer);

		//virtual void BeginDestroy() override;
	
};
