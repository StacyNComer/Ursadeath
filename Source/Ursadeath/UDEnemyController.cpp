// Fill out your copyright notice in the Description page of Project Settings.

#include "UDEnemyController.h"
#include "UDPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UDEnemy.h"

void AUDEnemyController::BeginPlay()
{
	Super::BeginPlay();

	//Run the AI's behavior tree.
	RunBehaviorTree(EnemyBehavior);

	//Tell the AI's Blackboard who the player character in play is.
	GetBlackboardComponent()->SetValueAsObject(FName("Player"), AUDPlayerCharacter::GetCharacterInPlay(GetWorld()));
}

/** Instantly stops the Enemy's AI.*/
void AUDEnemyController::StopAI()
{
	BrainComponent->StopLogic("Stunned");
}

/** Resumes the enemy AI if it was stopped before. */
void AUDEnemyController::ResumeAI()
{
	BrainComponent->StartLogic();
}