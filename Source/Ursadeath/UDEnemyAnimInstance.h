// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UDEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class URSADEATH_API UUDEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = Status)
		bool bEnemyDead;

	UPROPERTY(BlueprintReadOnly, Category = Status)
		bool bEnemyStunned;
};
