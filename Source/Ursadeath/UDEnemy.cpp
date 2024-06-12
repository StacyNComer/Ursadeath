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
#include "GameFramework/FloatingPawnMovement.h"


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

	//Create the slow particle system component.
	SlowParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SlowParticles"));
	SlowParticleComponent->SetupAttachment(SceneRoot);
	//Set the enemy slow particle system to its default value.
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SlowParticleSystemFinder(TEXT("/Game/Ursadeath/Enemies/Core/ParticleFX/NS_EnemySlow"));
	SlowParticleComponent->SetAsset(SlowParticleSystemFinder.Object);
	//Set the slow particles to start deactivated.
	SlowParticleComponent->bAutoActivate = false;

	SlowedSpeedScalar = 0.6f;
}

// Called when the game starts or when spawned
void AUDEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Cache the pawn's floating movement component if it has one.
	//Since the base pawn movement component cannot have its max speed change (at least as far as I know) we simply assume that any enemy that can move has a floating pawn movement component.
	if (GetMovementComponent())
	{
		FloatingPawnMovement = Cast<UFloatingPawnMovement>(GetMovementComponent());
		
		if (!FloatingPawnMovement)
		{
			//Print a message incase a different movement component is eventually used. 	
			//TODO: Remove this message and perhaps edit the code when all of the movement components being used are finalized.
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("This pawn has a movement component other than the Floating Pawn Movement. Tell Stacy so that he can adjust the source code for this. The enemy should work fine, except it can't recieve the 'Slow' effect."));
			}
		}
	}

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

	//Decrement the enemy's slow time.
	if (IsSlowed())
	{
		SlowTime -= DeltaTime;

		//The enemy's speed returns to normal once the slow time is depleted.
		if (SlowTime <= 0)
		{
			FloatingPawnMovement->MaxSpeed /= SlowedSpeedScalar;

			//Turn off the slow particle FX.
			SlowParticleComponent->Deactivate();
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
		Health -= AttackData->AttackStats.Damage;

		//Play the damage sound.
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, GetActorLocation());
	}

	//Stun the enemy if the attack should stun them.
	float AttackStunTime = AttackData->AttackStats.StunTime; 
	if (AttackStunTime > 0)
	{
		ApplyStun(AttackStunTime);
	}

#if WITH_EDITOR
	//If PrintDamageTaken is enabled, print the damage/stun that an enemy has taken and their remaining health.
	if (PrintDamageTaken)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString("Damage: ") + FString::SanitizeFloat(AttackData->AttackStats.Damage));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString("StunTime: ") + FString::SanitizeFloat(AttackData->AttackStats.StunTime));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString("Remaining Health: ") + FString::SanitizeFloat(Health));
	}
#endif
	
	bool WasKilled = Health <= 0;

	//Report that an enemy was hit to the attack's owning player.
	if (AttackSource)
	{
		AttackSource->GetOwningPlayer()->NotifyOnEnemyHit(this, AttackSource, WasKilled);
	}

	//Kill the enemy if they're health is 0 and they aren't immune to their otherwise inevitable demise.
	if (!bUndieable && Health <= 0)
	{
		Destroy();
	}
}

const bool AUDEnemy::IsStunned()
{
	return StunTime > 0;
}

void AUDEnemy::ApplySlow(float TimeSlowed)
{
	//The enemy can only be slowed for now if it has a floating pawn movement component. This value is obtained in Begin Play.
	if (FloatingPawnMovement)
	{
		//Slow the enemy's movement for the given time. If the enemy is already slowed, it's remaining time slowed is still set to Time Slowed if it is higher than the remaining time.
		if (!IsSlowed())
		{
			//Set how long the enemy should be slowed for.
			SlowTime = TimeSlowed;

			//Slow the enemy's movement
			FloatingPawnMovement->MaxSpeed *= SlowedSpeedScalar;

			SlowParticleComponent->Activate();

		}
		else if (TimeSlowed > SlowTime)
		{
			SlowTime = TimeSlowed;
		}
	}
}

const bool AUDEnemy::IsSlowed()
{
	return SlowTime > 0;
}

const float AUDEnemy::GetSpawnTime()
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

		if (UPawnMovementComponent* MoveComponent = GetMovementComponent())
		{
			GetMovementComponent()->StopActiveMovement();
		}
		
		
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
