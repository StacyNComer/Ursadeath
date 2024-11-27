// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UDCoreLibrary.generated.h"

class AUDEnemy;

/**
 * 
 */
UCLASS()
class URSADEATH_API UUDCoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	 UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static TArray<AUDEnemy*> GetNearestEnemies(const UObject* WorldContextObject, const int32 Count, const FVector Location);
};
