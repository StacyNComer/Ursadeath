// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerUpgrade.h"
#include "UDPlayerTickedUpgrade.generated.h"

/**
 * 
 */
UCLASS()
class URSADEATH_API UUDPlayerTickedUpgrade : public UUDPlayerUpgrade
{
	GENERATED_BODY()
	
protected:
	virtual void AttachToPlayer(AUDPlayerCharacter* Player) override;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void TickUpgrade(float DeltaTime);

};
