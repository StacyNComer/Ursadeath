// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UDEnemyUpgrade.generated.h"

class AUDEnemy;
struct FEnemySpawnData;

/**
 * 
 */
UCLASS(abstract, Blueprintable, BlueprintType)
class URSADEATH_API UUDEnemyUpgrade : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FEnemySpawnData Data;

public:
	virtual UWorld* GetWorld() const override;

	/** Applies the upgrade to the given enemy. Upgrades may be applied to an idefinite amount of enemies.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnUpgradeApplied(AUDEnemy* Enemy);

	UFUNCTION(BlueprintCallable)
		/** Creates an uprade instance of the given type with the given UObject as its "Outer"*/
		static UUDEnemyUpgrade* CreateUpgrade(TSubclassOf<UUDEnemyUpgrade> UpgradeClass, UObject* Outer);

		UFUNCTION(BlueprintNativeEvent)
		FEnemySpawnData OnEnemyDataChangeApplied(FEnemySpawnData OriginalData) const;

		//virtual void BeginDestroy() override;

protected:
		virtual FEnemySpawnData OnEnemyDataChangeApplied_Implementation(FEnemySpawnData OriginalData) const;

};
