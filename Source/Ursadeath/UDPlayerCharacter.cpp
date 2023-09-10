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
#include "UDPlayerController.h"
#include "UDPlayerHUDWidget.h"
#include "UDPlayerCooldownAbility.h"
#include "UDPlayerEnergyAbility.h"

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

	//Create attack spawn component
	AttackSpawnComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("AttackSpawn"));
	AttackSpawnComponent->SetupAttachment(FirstPersonCameraComponent);
	AttackSpawnComponent->SetRelativeLocation(FVector(50, 0, 0));

	//Initialize Ability Components. The abilities are give their actual inputs, attack actors, and cooldowns in blueprints. Their input is setup in SetupPlayerInput.
	PrimaryFireAbility = CreateDefaultSubobject<UUDPlayerCooldownAbility>(TEXT("PrimaryFireAbility"));
	MeleeAbility = CreateDefaultSubobject<UUDPlayerCooldownAbility>(TEXT("MeleeAbility"));
	RocketAbility = CreateDefaultSubobject<UUDPlayerEnergyAbility>(TEXT("RocketAbility"));
	ShockwaveAbility = CreateDefaultSubobject<UUDPlayerEnergyAbility>(TEXT("ShockwaveAbility"));

	//Set the player's default stats
	MaxHealth = 100;
	CurrentHealth = MaxHealth;

	MaxEnergy = 400;
	CurrentEnergy = 0;

	MeleePrimaryPause = .5f;
}

void AUDPlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Set the UDPlayerController.
	UDPlayerController = Cast<AUDPlayerController>(GetController());

	// Cache the player HUD from the player controller.
	PlayerHUDWidget = UDPlayerController->PlayerHUDWidget;
	// Set the HUD to display the players starting stats.
	PlayerHUDWidget->UpdateHealth(CurrentHealth);
	PlayerHUDWidget->UpdateEnergy(CurrentEnergy);

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
	Super::Tick(deltaTime);
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
		EnhancedInputComponent->BindAction(PrimaryFireAbility->GetInputAction(), PrimaryFireAbility->GetInputTrigger(), this, &AUDPlayerCharacter::UsePrimaryAbility);

		//Melee Attack
		EnhancedInputComponent->BindAction(MeleeAbility->GetInputAction(), MeleeAbility->GetInputTrigger(), this, &AUDPlayerCharacter::UseMeleeAbility);

		//Firing Rockets.
		EnhancedInputComponent->BindAction(RocketAbility->GetInputAction(), RocketAbility->GetInputTrigger(), this, &AUDPlayerCharacter::UseRocketAbility);

		//Firing Shockwaves
		EnhancedInputComponent->BindAction(ShockwaveAbility->GetInputAction(), ShockwaveAbility->GetInputTrigger(), this, &AUDPlayerCharacter::UseShockwaveAbility);
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
		ActorSpawnParams.Owner = this;

		//Spawn the attack. Set the player spawning it as the attack's owner.
		AUDPlayerAttack* attackSpawned = World->SpawnActor<AUDPlayerAttack>(attackClass, AttackSpawnComponent->GetComponentLocation(), AttackSpawnComponent->GetComponentRotation(), ActorSpawnParams);
	}
}

void AUDPlayerCharacter::AddEnergy(float Value)
{
	CurrentEnergy += Value;

	//Make sure the Current Energy is capped at max energy.
	if (CurrentEnergy > MaxEnergy)
	{
		CurrentEnergy = MaxEnergy;
	}

	PlayerHUDWidget->UpdateEnergy(CurrentEnergy);
}

float AUDPlayerCharacter::GetEnergy()
{
	return CurrentEnergy;
}

void AUDPlayerCharacter::ExpendEnergy(float ToExpend)
{
	CurrentEnergy -= ToExpend;

	PlayerHUDWidget->UpdateEnergy(CurrentEnergy);
}

void AUDPlayerCharacter::UsePrimaryAbility()
{
	PrimaryFireAbility->TryUseAbility();
}

void AUDPlayerCharacter::UseMeleeAbility()
{
	if (MeleeAbility->TryUseAbility())
	{
		PrimaryFireAbility->ForceCooldown(MeleePrimaryPause);
	}
}

void AUDPlayerCharacter::UseRocketAbility()
{
	RocketAbility->TryUseAbility();
}

void AUDPlayerCharacter::UseShockwaveAbility()
{
	ShockwaveAbility->TryUseAbility();
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