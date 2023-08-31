// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemy.h"
#include "Components/MeshComponent.h"
#include "UDPlayerAttack.h"
#include "UDPlayerAttackData.h"

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
		//If SpawnInstantly is set, EndSpawnSequence is calle
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

float AUDEnemy::GetSpawnTime()
{
	return SpawnTime;
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
