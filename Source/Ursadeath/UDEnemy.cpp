// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemy.h"
#include "Components/MeshComponent.h"
#include "UDPlayerAttack.h"
#include "UDPlayerAttackData.h"
#include "UDEnemyController.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AUDEnemy::AUDEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Prevents the pawn's AI from acting during its spawn sequence unless this sequence is set to be skipped.
	if (!bSpawnInstantly)
	{
		AutoPossessAI = EAutoPossessAI::Disabled;
	}

	//Set the default spawning material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SpawningMaterialFinder(TEXT("/Game/Ursadeath/Enemies/Core/M_SpawningEnemy"));
	SpawningMaterial = SpawningMaterialFinder.Object;
}

// Called when the game starts or when spawned
void AUDEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnInstantly)
	{
		//If SpawnInstantly is set, spawn the controller on begin play.
		SpawnDefaultController();
	}
	else
	{
		GetComponents(EnemyMeshes);

		SetActorEnableCollision(false);

		//Set the enemy meshes to have the spawning material.
		SetEnemyMaterials(SpawningMaterial);

		//Sets a timer that completes the enemy's spawn sequence in SpawningTime seconds.
		FTimerHandle SpawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AUDEnemy::EndSpawnSequence, SpawnTime, false);
	}
}

// Called every frame
void AUDEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Decrement the enemy's stun time.
	if (IsStunned())
	{
		StunTime -= DeltaTime;

		//The enemy's AI resumes once their stun time reaches 0.
		if (StunTime <= 0)
		{
			EnemyController->ResumeAI();
		}
	}
}

bool AUDEnemy::ReceiveAttack(UUDPlayerAttackData* AttackData)
{
	//Call the attack recieved event.
	if(OnAttackRecieved.IsBound())
	{
		OnAttackRecieved.Broadcast(AttackData);
	}

	//Reduce the enemy's health
	health -= AttackData->AttackStats.Damage;

	//Stun the enemy if the attack should stun them.
	float AttackStunTime = AttackData->AttackStats.StunTime; 
	if (AttackStunTime > 0)
	{
		ApplyStun(AttackStunTime);
	}

	//Kill the enemy if they're health is 0 and they aren't immune to their otherwise inevitable demise.
	if (!bUndieable && health <= 0)
	{
		Destroy();
		return true;
	}
	else
	{
		return false;
	}
}

bool AUDEnemy::IsStunned()
{
	return StunTime > 0;
}

float AUDEnemy::GetSpawnTime()
{
	return SpawnTime;
}

EEnemyTier AUDEnemy::GetEnemyTier()
{
	return EnemyTier;
}

void AUDEnemy::SetEnemyTier(EEnemyTier Tier)
{
	EnemyTier = Tier;
}

void AUDEnemy::ApplyStun(float TimeStunned)
{
	//If the player was not stunned before stun them. If they were already stunned, only apply the stun if it would be longer.
	if (!IsStunned())
	{
		StunTime = TimeStunned;

		EnemyController->StopAI();
		GetMovementComponent()->StopActiveMovement();
	}
	else if (TimeStunned > StunTime)
	{
		StunTime = TimeStunned;
	}
}

void AUDEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EnemyController = Cast<AUDEnemyController>(NewController);
}

void AUDEnemy::EndSpawnSequence()
{
	//Give the enemy its AI.
	SpawnDefaultController();

	//Give the enemy its collision
	SetActorEnableCollision(true);
	
	//Reset the enemy's mesh materials.
	SetEnemyMaterials(NULL);

	OnSpawnSequenceEnd();

}

void AUDEnemy::SetEnemyMaterials(UMaterialInterface* newMaterial)
{	
	for (int i = 0; i < EnemyMeshes.Num(); i++)
	{
		for (int j = 0; j < EnemyMeshes[i]->GetMaterials().Num(); j++)
		{
			EnemyMeshes[i]->SetMaterial(j, newMaterial);
		}
	}
}
