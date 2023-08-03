// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrsadeathCharacter.h"
#include "UDPlayerAttack.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////// AUrsadeathCharacter

AUrsadeathCharacter::AUrsadeathCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	//Creat attack spawn component
	AttackSpawnComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("AttackSpawn"));
	AttackSpawnComponent->SetupAttachment(FirstPersonCameraComponent);
	AttackSpawnComponent->SetRelativeLocation(FVector(50,0,0));

	//Initialize Attacks
	bFiringPrimary = false;
}

void AUrsadeathCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Cache PlayerController
	playerController = Cast<APlayerController>(GetController());

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AUrsadeathCharacter::Tick(float deltaTime)
{
	//Tick down the cooldown of the player's primary fire. If the player's primary is off cooldown while they are firing their weapon, spawn the primary projectile.
	if (primaryCooldownTracker > 0)
	{
		primaryCooldownTracker -= deltaTime;
	}
	else if(bFiringPrimary)
	{
		SpawnAttack(PrimaryAttackActor);

		//Put the primary attack on cooldown.
		primaryCooldownTracker = primaryCooldown;
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AUrsadeathCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUrsadeathCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUrsadeathCharacter::Look);

		//Primary Fire
		EnhancedInputComponent->BindAction(FirePrimaryAction, ETriggerEvent::Triggered, this, &AUrsadeathCharacter::StartFiringPrimary);
		EnhancedInputComponent->BindAction(FirePrimaryAction, ETriggerEvent::Completed, this, &AUrsadeathCharacter::StopFiringPrimary);
	}
}

void AUrsadeathCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AUrsadeathCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AUrsadeathCharacter::SpawnAttack(const TSubclassOf<AUDPlayerAttack> attackClass)
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Spawn the attack. Set the player spawning it as the attack's owner.
		AUDPlayerAttack* attackSpawned = World->SpawnActor<AUDPlayerAttack>(attackClass, AttackSpawnComponent->GetComponentLocation(), AttackSpawnComponent->GetComponentRotation(), ActorSpawnParams);
		attackSpawned->Owner = this;
	}
}

void AUrsadeathCharacter::StartFiringPrimary()
{
	bFiringPrimary = true;
}

void AUrsadeathCharacter::StopFiringPrimary()
{
	bFiringPrimary = false;
}

void AUrsadeathCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AUrsadeathCharacter::GetHasRifle()
{
	return bHasRifle;
}