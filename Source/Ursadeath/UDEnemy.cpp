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
#include "Components/SkeletalMeshComponent.h"
#include "UDEnemyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "UDEnemyUpgrade.h"


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

	EnemyCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Enemy Collision"));
	EnemyCollision->SetupAttachment(SceneRoot);
	EnemyCollision->SetCollisionProfileName("EnemyPawn");
	EnemyCollision->bDynamicObstacle = true;

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

	//Spawn the root component for the Ward VFX.
	WardFXRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WardFXRoot"));
	WardFXRoot->SetupAttachment(SceneRoot);

	SlowedSpeedScalar = 0.6f;
}

// Called when the game starts or when spawned
void AUDEnemy::BeginPlay()
{
	Super::BeginPlay();

	//If the enemy didn't spawn with any Ward, make the Ward FX invisible.
	WardFXRoot->SetVisibility(Ward > 0, true);

	//Set the default size for the stun particles.
	StunParticleComponent->SetFloatParameter(TEXT("User.SpawnRadius"), StunParticleRadius);

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

	//Get the enemy's anim instance if it has one so that it can recieve any relevant changes in state from the enemy.
	USkeletalMeshComponent* SkeletonComp = GetComponentByClass<USkeletalMeshComponent>();
	if (SkeletonComp)
	{
		EnemyAnimInstance = Cast<UUDEnemyAnimInstance>(SkeletonComp->GetAnimInstance());
	}

	//If the enemy is set to spawn instantly, give it's controller/AI. Otherwise, the enemy will take a moment to "spawn in", during which it has niether collision nor an AI/Controller.
	if (bSpawnInstantly)
	{
		//If SpawnInstantly is set, spawn the controller on begin play.
		SpawnDefaultController();

		ValidEnemy = true;
	}
	else
	{
		SetActorEnableCollision(false);

		GetComponents(EnemyMeshes);

		//Set the enemy meshes to have the spawning material.
		SetEnemyMaterials(SpawningMaterial);

		//Sets a timer that completes the enemy's spawn sequence in SpawningTime seconds.
		FTimerHandle SpawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AUDEnemy::EndSpawnSequence, SpawnTime, false);
	}

	//If the enemy has a debu upgrade, give them that upgrade.
	if (DebugUpgrade)
	{
		UUDEnemyUpgrade* Upgrade = UUDEnemyUpgrade::CreateUpgrade(DebugUpgrade, this);

		Upgrade->OnUpgradeApplied(this);
	}
}

// Called every frame
void AUDEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//What to do each tick that the enemy is alive. 
	// Note that if any enemy is set to be "Undieable", then these will still occur even if the enemy's health is below 0.
	if (!IsDead())
	{
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

				//If the enemy has animations, turn off its "Stunned" state.
				if (EnemyAnimInstance)
				{
					EnemyAnimInstance->bEnemyStunned = false;
				}
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
}

void AUDEnemy::ReceiveAttack(UUDPlayerAttackData* AttackData, AUDPlayerAttack* AttackSource)
{
	//Call the attack recieved event. IMPORTANT: this event is used to calculate damage weaknesses and must be perfored first for damage/stun values to be accurate.
	if (OnAttackRecieved.IsBound())
	{
		OnAttackRecieved.Broadcast(AttackData);
	}

	int32 AttackDamage = AttackData->AttackStats.Damage;
	float AttackStunTime = AttackData->AttackStats.StunTime;

	//Ward resistances
	if (Ward > 0)
	{
		//Enemies are immune to rockets and shockwaves while warded!
		if (AttackData->AttackStats.AttackType == EPlayerAttackType::ROCKET || AttackData->AttackStats.AttackType == EPlayerAttackType::SHOCKWAVE)
		{
			AttackDamage = 0;
			AttackStunTime = 0;
		}
	}
		
	//Calculate if the enemy was killed by the attack. We do this before actually damaging them to make sure they weren't already dead.
	bool WasKilled = Health > 0 && (Health + Ward - AttackDamage) <= 0;


	//If the attack deals damage, damage the enemy and play a hit sound.
	if (AttackDamage > 0)
	{
		//If the enemy has any amount of Ward, have it absorb the damage. Otherwise, only the health needs to be affected.
		if (Ward > 0)
		{
			Ward -= AttackDamage;

			//If the damage exceeds the Ward amount (i.e. the damage reduces it below zero), the negative value (or "overflow") is taken from the enemy's health.
			if (Ward <= 0)
			{
				//We add because the Ward amount will be nega
				Health += Ward;

				//Since we take the Ward value into account when deciding whether or not an enemy has died, we must set the ward to 0 to prevent the enemy from dying prematurely.
				Ward = 0;

				//Turn of the WardFX
				WardFXRoot->SetVisibility(false, true);
			}
		}
		else
		{
			Health -= AttackDamage;
		}

		//Play the damage sound.
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, GetActorLocation());
	}

	//Stun the enemy if the attack should stun them.
	if (!WasKilled && AttackStunTime > 0)
	{
		ApplyStun(AttackStunTime);
	}

	//Report that an enemy was hit to the attack's owning player.
	if (AttackSource)
	{
		AttackSource->GetOwningPlayer()->NotifyOnEnemyHit(this, AttackSource, WasKilled);
	}

	//Kill the enemy if they're health is 0 and they aren't immune to their otherwise inevitable demise.
	if (!bUndieable && WasKilled)
	{
		ValidEnemy = false;

		//Make it so that the enemy no longer collides with pawns or attacks.
		EnemyCollision->SetCollisionProfileName("DeadEnemyPawn");

		//Stop the enemy's AI and movement.
		EnemyController->StopAI();
		if(UPawnMovementComponent* MoveComponent = GetMovementComponent())
		{
			GetMovementComponent()->StopActiveMovement();
		}

		//Disable any status effect related particles when the enemy dies.
		StunParticleComponent->Deactivate();
		SlowParticleComponent->Deactivate();

		//Interrupt the enemy as if they were stunned. This should cause any ongoing attacks not reliant on the AI to end, assuming this event is implemented correctly.
		OnInterrupted(IsStunned());

		//Broadcast the OnEnemyKilled event.
		if (OnEnemyKilled.IsBound())
		{
			OnEnemyKilled.Broadcast(this);
		}

			//Death Anims/FX

		//Tell the enemy's animation script that its owner has died.
		if (EnemyAnimInstance)
		{
			EnemyAnimInstance->bEnemyDead = true;
		}

		if (DeathSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
		}
	}

#if WITH_EDITOR
	//If PrintDamageTaken is enabled, print the damage/stun that an enemy has taken and their remaining health.
	if (PrintDamageTaken)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString("Damage: ") + FString::SanitizeFloat(AttackDamage));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString("StunTime: ") + FString::SanitizeFloat(AttackStunTime));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString("Remaining Ward: ") + FString::SanitizeFloat(Ward));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString("Remaining Health: ") + FString::SanitizeFloat(Health));

	}
#endif
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

const bool AUDEnemy::IsDead()
{
	return Health <= 0 && !bUndieable;
}

bool AUDEnemy::IsValidEnemy() const
{
	return ValidEnemy;
}

void AUDEnemy::ApplyWard(int32 WardAmount)
{
	Ward += WardAmount;

	WardFXRoot->SetVisibility(true, true);
}

void AUDEnemy::SetEnemyTier(EEnemyTier Tier)
{
	EnemyTier = Tier;
}

void AUDEnemy::ApplyStun(float TimeStunned)
{
	//Notify the blueprint that this enemy was stunned.
	OnInterrupted(IsStunned());

	//If the player was not stunned before stun them. If they were already stunned, only apply the stun if it would be longer.
	if (!IsStunned())
	{
		StunTime = TimeStunned;

		//Pause the enemy AI.
		EnemyController->StopAI();

		//Cancel any active movement
		if (UPawnMovementComponent* MoveComponent = GetMovementComponent())
		{
			GetMovementComponent()->StopActiveMovement();
		}

		//If the enemy has animations, turn on its "Stunned" state.
		if (EnemyAnimInstance)
		{
			EnemyAnimInstance->bEnemyStunned = true;
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
	ValidEnemy = true;

	//Give the enemy its AI.
	SpawnDefaultController();

	//Give the enemy its collision
	SetActorEnableCollision(true);
	
	//Reset the enemy's mesh materials to their default values.
	SetEnemyMaterials(NULL);

	//Tell the blueprint side of the script that the enemy has spawned.
	OnSpawnSequenceEnd();
}

void AUDEnemy::SetEnemyMaterials(UMaterialInterface* newMaterial)
{	
	for (int i = 0; i < EnemyMeshes.Num(); i++)
	{
		if (EnemyMeshes[i]->GetAttachParent() != WardFXRoot)
		{
			//We iterate throught the meshes materials in case it has more than one material.
			for (int j = 0; j < EnemyMeshes[i]->GetMaterials().Num(); j++)
			{

				EnemyMeshes[i]->SetMaterial(j, newMaterial);

			}
		}
	}
}
