// Copyright Epic Games, Inc. All Rights Reserved.

#include "UDPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UDPlayerAttack.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//////////////////////////////////////////////////////////////////////////// AUrsadeathCharacter

AUDPlayerCharacter::AUDPlayerCharacter()
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
	AttackSpawnComponent->SetRelativeLocation(FVector(50, 0, 0));
}

void AUDPlayerCharacter::BeginPlay()
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

void AUDPlayerCharacter::Tick(float deltaTime)
{
	//Tick down the cooldown of the player's primary fire.
	if (primaryCooldownTracker > 0)
	{
		primaryCooldownTracker -= deltaTime;
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AUDPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUDPlayerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUDPlayerCharacter::Look);

		//Primary Fire
		EnhancedInputComponent->BindAction(PrimaryFireAbility.InputAction, ETriggerEvent::Triggered, this, &AUDPlayerCharacter::FirePrimary);

		//"Start" is used for Rockets and Shockwaves to make them "Semi-automatic".
		//Firing Rockets.
		EnhancedInputComponent->BindAction(RocketAbility.InputAction, ETriggerEvent::Started, this, &AUDPlayerCharacter::FireRocket);
	}
}

void AUDPlayerCharacter::Move(const FInputActionValue& Value)
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

void AUDPlayerCharacter::Look(const FInputActionValue& Value)
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

void AUDPlayerCharacter::SpawnAttack(const TSubclassOf<AUDPlayerAttack> attackClass)
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

void AUDPlayerCharacter::FirePrimary()
{
	if (primaryCooldownTracker <= 0)
	{
		SpawnAttack(PrimaryFireAbility.AttackActorClass);

		//Put the primary attack on cooldown.
		primaryCooldownTracker = primaryCooldown;
	}
}


void AUDPlayerCharacter::FireRocket()
{
	SpawnAttack(RocketAbility.AttackActorClass);
}

AUDPlayerCharacter* AUDPlayerCharacter::GetCharacterInPlay(UObject* WorldContextObject)
{
	return Cast<AUDPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0));
}

void AUDPlayerCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AUDPlayerCharacter::GetHasRifle()
{
	return bHasRifle;
}