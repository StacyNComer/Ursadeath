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

private:
	inline static float ArenaDiameter = 4500;

public:
	 UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsIgnored"))
		static TArray<AUDEnemy*> GetNearestEnemies(const UObject* WorldContextObject, TArray<AActor*> ActorsIgnored, const int32 Count, const int32 MaxSearchAttempts, const FVector Location, const bool bDrawDebug);

	//Returns the diameter of the arena, with some extra breathing room added in. Useful for traces that could potentially span the entire arena, such as beam attacks.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetArenaDiameter();
};
