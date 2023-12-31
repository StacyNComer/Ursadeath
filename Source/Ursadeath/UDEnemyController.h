// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UDEnemyController.generated.h"

class UBehaviorTree;

/**
 * The base class for every enemy's AI Controller. Make sure to set the behavior tree that the enemy will have!
 */
UCLASS(Abstract)
class URSADEATH_API AUDEnemyController : public AAIController
{
	GENERATED_BODY()

protected:
	/** The behavior tree the AI will run upon creation.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UBehaviorTree> EnemyBehavior;

public:
	void StopAI();

	void ResumeAI();

protected:
	virtual void BeginPlay();
	
};
