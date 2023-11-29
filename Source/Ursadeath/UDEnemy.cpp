// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemy.h"
#include "Components/MeshComponent.h"
#include "UDPlayerAttack.h"
#include "UDPlayerAttackData.h"
#include "UDEnemyController.h"
#include "UDPlayerCharacter.h"
#include "NiagaraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

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

	//Create a root component for this actors components to be attached to.
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	//Set the default spawning material.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SpawningMaterialFinder(TEXT("/Game/Ursadeath/Enemies/Core/Materials/M_SpawningEnemy"));
	SpawningMaterial = SpawningMaterialFinder.Object;

	//Create the stun particle system component.
	StunParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunParticles"));
	StunParticleComponent->SetupAttachment(SceneRoot);
	
	//Set the enemy stun particle system to its default value.
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> StunParticleSystemFinder(TEXT("/Game/Ursadeath/Enemies/Core/ParticleFX/NS_EnemyStun"));
	StunParticleComponent->SetAsset(StunParticleSystemFinder.Object);

	//Set the stun particles to start deactivated.
	StunParticleComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AUDEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Set the default size for the stun particles.
	StunParticleComponent->SetFloatParameter(TEXT("User.SpawnRadius"), StunParticleRadius);


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

			//Turn off the stun particle FX.
			StunParticleComponent->Deactivate();

		}
	}
}

void AUDEnemy::ReceiveAttack(UUDPlayerAttackData* AttackData, AUDPlayerAttack* AttackSource)
{
	//Call the attack recieved event.
	if (OnAttackRecieved.IsBound())
	{
		OnAttackRecieved.Broadcast(AttackData);
	}

	//If the attack deals damage, damage the enemy and play a hit sound.
	if (AttackData->AttackStats.Damage > 0)
	{
		health -= AttackData->AttackStats.Damage;

		UGameplayStatics::PlaySound2D(GetWorld(), DamageSound);
	}

	//Stun the enemy if the attack should stun them.
	float AttackStunTime = AttackData->AttackStats.StunTime; 
	if (AttackStunTime > 0)
	{
		ApplyStun(AttackStunTime);
	}

	//Kill the enemy if they're health is 0 and they aren't immune to their otherwise inevitable demise.
	if (!bUndieable && health <= 0)
	{
		
		//Report the kill to the attacking player, if they exist.
		if (AttackSource)
		{
			AttackSource->GetOwningPlayer()->NotifyOnEnemyKill(this, AttackSource);
		}

		Destroy();
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
	//Notify the blueprint that this enemy was stunned.
	OnStunned(IsStunned());

	//If the player was not stunned before stun them. If they were already stunned, only apply the stun if it would be longer.
	if (!IsStunned())
	{
		StunTime = TimeStunned;

		//Pause the enemy AI and cancel any active movement.
		EnemyController->StopAI();
		GetMovementComponent()->StopActiveMovement();
		
		//Turn on the stun particle FX.
		StunParticleComponent->Activate();
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
