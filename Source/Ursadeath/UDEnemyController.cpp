// Fill out your copyright notice in the Description page of Project Settings.

#include "UDEnemyController.h"
#include "UDPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

void AUDEnemyController::BeginPlay()
{
	Super::BeginPlay();

	//Run the AI's behavior tree.
	RunBehaviorTree(enemyBehavior);

	//Tell the AI's Blackboard who the player character in play is.
	GetBlackboardComponent()->SetValueAsObject(FName("Player"), AUDPlayerCharacter::GetCharacterInPlay(GetWorld()));
}