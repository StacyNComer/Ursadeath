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
#include "UDEnemy.h"
#include "UrsadeathGameInstance.h"
#include "UDArena.h"
#include "Components/Button.h"
#include "UDRoundScreenWidget.h"
#include "UDGameOverWidget.h"
#include "Sound/SoundBase.h"
#include "UDPlayerUpgrade.h"
#include "UDPlayerTickedUpgrade.h"
#include "UDPlayerStatusIcon.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UDHealthPickup.h"
#include "UDPauseMenuWidget.h"

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

	bInfiniteEnergy = false;
}

void AUDPlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Get the game instance.
	UrsadeathGameInstance = GetGameInstance<UUrsadeathGameInstance>();

	// Set the UDPlayerController.
	UDPlayerController = GetController<AUDPlayerController>();

	//Create player hud
	HUDWidget = CreateWidget<UUDPlayerHUDWidget>(UDPlayerController, PlayerHUDWidgetClass);
	HUDWidget->AddToViewport();

	// Set the HUD to display the players starting stats.
	HUDWidget->UpdateHealth(CurrentHealth, MaxHealth, 0);
	HUDWidget->UpdateEnergy(CurrentEnergy, MaxEnergy, 0, false);

	//Create the status icons for being hasted and invulnerable.
	HasteStatusWidget = HUDWidget->AddPlayerStatusIcon();
	HasteStatusWidget->SetStatusImage(HasteStatusImage);

	InvulnerableStatusWidget = HUDWidget->AddPlayerStatusIcon();
	InvulnerableStatusWidget->SetStatusImage(InvulnerableStatusImage);

	//Create the round screen.
	RoundScreenWidget = CreateWidget<UUDRoundScreenWidget>(UDPlayerController, RoundScreenWidgetClass);
	RoundScreenWidget->AddToViewport();
	//The round screen should be closed by default.
	RoundScreenWidget->SetVisibility(ESlateVisibility::Collapsed);

	//Create the game over widget.
	GameOverWidget = CreateWidget<UUDGameOverWidget>(UDPlayerController, GameOverWidgetClass);
	GameOverWidget->AddToViewport();
	//Make sure the player isn' shown the game over screen the moment they load in!
	GameOverWidget->SetVisibility(ESlateVisibility::Collapsed);

	//Create the Pause Screen widget
	PauseMenuWidget = CreateWidget<UUDPauseMenuWidget>(UDPlayerController, PauseMenuWidgetClass);
	PauseMenuWidget->AddToViewport();
	//Make sure the game doesn't start paused
	PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	
	//Set the input subsystem
	InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(UDPlayerController->GetLocalPlayer());
		
	//Add Input Mapping Context
	InputSubsystem->AddMappingContext(PlayerCharacterMappingContext, 0);

	UrsadeathGameInstance->FinalizePlayerSetup(this);
}

void AUDPlayerCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	//Decrement the player's haste time. Their haste ends if the time is depleted to 0.
	if (HasteTimeTracker > 0)
	{
		HasteTimeTracker -= deltaTime;
		HasteStatusWidget->DecrementStatusTime(deltaTime);

		if (HasteTimeTracker <= 0)
		{
			GetCharacterMovement()->MaxWalkSpeed /= HasteSpeedMultiplier;
		}
	}

	//Decrement the player's invulnerability time.
	if (InvulnerabilityTimeTracker > 0)
	{
		InvulnerabilityTimeTracker -= deltaTime;
	}

	//Tick any upgrades the player has that requires such.
	for (UUDPlayerTickedUpgrade* TickedUpgrade : TickedUpgrades)
	{
		TickedUpgrade->TickUpgrade(deltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AUDPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Toggling the round menu
		EnhancedInputComponent->BindAction(RoundMenuAction, ETriggerEvent::Started, this, &AUDPlayerCharacter::ToggleRoundMenu);

		//Toggling the pause menu
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AUDPlayerCharacter::TogglePauseMenu);

		//Backing out of the pause menu
		EnhancedInputComponent->BindAction(UIBackAction, ETriggerEvent::Started, this, &AUDPlayerCharacter::BackOutPauseMenu);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUDPlayerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUDPlayerCharacter::Look);

		//Controller UI click
		EnhancedInputComponent->BindAction(ControllerClickAction, ETriggerEvent::Started, this, &AUDPlayerCharacter::PressUIViaController);
		EnhancedInputComponent->BindAction(ControllerClickAction, ETriggerEvent::Completed, this, &AUDPlayerCharacter::ReleaseUIViaController);

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

void AUDPlayerCharacter::ToggleRoundMenu()
{
	//Decide whether the round screen should be opening or closing. The round screen closes if its visibity is already set to "Visible", and opens if the visibility is set to ANYTHING else.
	bool RoundScreenOpening = RoundScreenWidget->GetVisibility() != ESlateVisibility::Visible;

	//Collapse the round screen if it is open. Make the round screen visible otherwise.
	ESlateVisibility NewRoundScreenVisibility = RoundScreenOpening ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	RoundScreenWidget->SetVisibility(NewRoundScreenVisibility);

	//Enable/Disable the Round Menu controls
	if (RoundScreenOpening)
	{
		InputSubsystem->AddMappingContext(RoundMenuMappingContext, 0);
	}
	else
	{
		InputSubsystem->RemoveMappingContext(RoundMenuMappingContext);
	}

	//Pause the game if the round screen is being opened. Unpause if it is being closed.
	SetIsInMenu(RoundScreenOpening);
}

void AUDPlayerCharacter::TogglePauseMenu()
{
	bool PauseScreenOpening = PauseMenuWidget->GetVisibility() != ESlateVisibility::Visible;

	//Collapse the round screen if it is open. Make the round screen visible otherwise.
	ESlateVisibility NewPauseScreenVisibility = PauseScreenOpening ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	PauseMenuWidget->SetVisibility(NewPauseScreenVisibility);

	//Enable/disable the pause menu controls.
	if (PauseScreenOpening)
	{
		InputSubsystem->AddMappingContext(PauseMenuMappingContext, 0);
	}
	else
	{
		InputSubsystem->RemoveMappingContext(PauseMenuMappingContext);
	}

	SetIsInMenu(PauseScreenOpening);
}

void AUDPlayerCharacter::BackOutPauseMenu()
{
	PauseMenuWidget->BackOutMenu();
}

void AUDPlayerCharacter::SetEnergy(float Value)
{
	//Record the change in energy for the UI update.
	float DeltaEnergy = Value - CurrentEnergy;

	//Check if the change in energy resulted in an energy bar being filled.
	bool EnergyBarGained = floorf(Value / 100) > floorf(CurrentEnergy / 100);
	
	//Set the player's energy
	CurrentEnergy = Value;

	if (EnergyBarGained && EnergyBarGainedSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EnergyBarGainedSound);
	}

	//Update the UI
	HUDWidget->UpdateEnergy(CurrentEnergy, MaxEnergy, DeltaEnergy, EnergyBarGained);
}

void AUDPlayerCharacter::AddEnergy(float ToAdd)
{
	float NewEnergy = CurrentEnergy + ToAdd;

	//Make sure the Current Energy is capped at max energy.
	if (NewEnergy > MaxEnergy)
	{
		NewEnergy = MaxEnergy;
	}

	SetEnergy(NewEnergy);
}

float AUDPlayerCharacter::GetCurrentEnergy()
{
	//If the player has Infinite Energy, return MaxEnergy. Otherwise, return their current energy.
	return bInfiniteEnergy? MaxEnergy : CurrentEnergy;
}

bool AUDPlayerCharacter::TestEnergy(float amount)
{
	return GetCurrentEnergy() >= amount;
}

bool AUDPlayerCharacter::TestEnergyWithFX(float amount)
{
	//If the player didn't have enough energy, play FX alerting the player to their poor resource management skillz.
	if (!TestEnergy(amount))
	{
		//Play a mean audio sound.
		if (NoEnergySound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), NoEnergySound);
		}
		
		return false;
	}

	return true;
}

void AUDPlayerCharacter::ExpendEnergy(float ToExpend)
{
	//Energy is only expended if the Infinite Energy is turned off.
	if (!bInfiniteEnergy)
	{
		float NewEnergy = CurrentEnergy - ToExpend;

		SetEnergy(NewEnergy);
	}
}

void AUDPlayerCharacter::SetHealth(int Value)
{
	int DeltaHealth = Value - CurrentHealth;

	CurrentHealth = Value;
	
	//Update the UI
	HUDWidget->UpdateHealth(CurrentHealth, MaxHealth, DeltaHealth);
}

void AUDPlayerCharacter::DamagePlayer(int Damage)
{
	//You can't hurt Ursadeath while he's invulnerable silly!
	if (IsInvulnerable())
	{
		return;
	}

	SetHealth(CurrentHealth - Damage);

	if (DamageSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DamageSound);
	}

	if (CurrentHealth <= 0)
	{
		//Show the game over widget and update it with the player's score.
		GameOverWidget->SetVisibility(ESlateVisibility::Visible);
		GameOverWidget->DisplayScore(UrsadeathGameInstance->GetCurrentRound(), UrsadeathGameInstance->GetAbsoluteWave());

		//Turn of the player's game controls because they are dead now. Turn on the menu controls
		InputSubsystem->RemoveMappingContext(PlayerCharacterMappingContext);
		InputSubsystem->AddMappingContext(GameMenuMappingContext, 0);

		UDPlayerController->SetInputMode(FInputModeGameAndUI());

		UDPlayerController->bShowMouseCursor = true;
	}
}

void AUDPlayerCharacter::AddUpgrade(UUDPlayerUpgrade* Upgrade)
{
	Upgrades.Add(Upgrade);
}

void AUDPlayerCharacter::AddTickedUpgrade(UUDPlayerTickedUpgrade* Upgrade)
{
	TickedUpgrades.Add(Upgrade);
}

void AUDPlayerCharacter::NotifyOnHealthPickupUsed(AUDHealthPickup* HealthPickup)
{
	RestoreHealth(40);

	if (OnHealthPickupUsed.IsBound())
	{
		OnHealthPickupUsed.Broadcast(HealthPickup);
	}
}

void AUDPlayerCharacter::RestoreHealth(int ToRestore)
{
	float NewHealth = CurrentHealth + ToRestore;

	//Make sure the Current Health is capped at max Health.
	if (NewHealth > MaxHealth)
	{
		NewHealth = MaxHealth;
	}

	SetHealth(NewHealth);
}

void AUDPlayerCharacter::ApplyHaste(float Time)
{
	if (HasteTimeTracker <= 0)
	{
		GetCharacterMovement()->MaxWalkSpeed *= HasteSpeedMultiplier;
	}

	if (Time > HasteTimeTracker)
	{
		HasteTimeTracker = Time;

		//Show in the player's HUD that they are Hasted for the given time.
		HasteStatusWidget->StartStatusBar(Time);
	}
}

void AUDPlayerCharacter::ApplyInvulnerability(float Time)
{
	if (InvulnerabilityTimeTracker < Time)
	{
		InvulnerabilityTimeTracker = Time;

		//Show in the player's HUD that they are invuln for the given time.
		InvulnerableStatusWidget->StartStatusBar(Time);
	}
}

bool AUDPlayerCharacter::IsInvulnerable()
{
	return bPermanentInvulnerability || InvulnerabilityTimeTracker > 0;
}

void AUDPlayerCharacter::SpawnAttack(const TSubclassOf<AUDPlayerAttack> AttackClass, FVector Location, FRotator Rotation)
{
	FActorSpawnParameters ActorSpawnParams;
	//Set Spawn Collision Handling Override
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//Set the attack's owner.
	ActorSpawnParams.Owner = this;

	//Spawn the attack. Set the player spawning it as the attack's owner.
	AUDPlayerAttack* AttackSpawned = GetWorld()->SpawnActor<AUDPlayerAttack>(AttackClass, Location, Rotation, ActorSpawnParams);

	if (OnAttackSpawned.IsBound())
	{
		OnAttackSpawned.Broadcast(AttackSpawned);
	}

	AttackSpawned->FinalizeAttack();
}


void AUDPlayerCharacter::SpawnAttackFromPlayer(const TSubclassOf<AUDPlayerAttack> AttackClass)
{
	SpawnAttack(AttackClass, AttackSpawnComponent->GetComponentLocation(), AttackSpawnComponent->GetComponentRotation());
}

void AUDPlayerCharacter::NotifyOnEnemyHit(AUDEnemy* Enemy, AUDPlayerAttack* Attack, bool EnemyWasKilled)
{
	if (OnEnemyHit.IsBound())
	{
		FEnemyHitData EnemyHitData;
		EnemyHitData.Enemy = Enemy;
		EnemyHitData.Attack = Attack;
		EnemyHitData.EnemyTier = Enemy->GetEnemyTier();
		EnemyHitData.EnemyWasKilled = EnemyWasKilled;

		OnEnemyHit.Broadcast(EnemyHitData);
	}

	if (EnemyWasKilled)
	{
		HUDWidget->DecrementEnemyCount(Enemy->GetClass(), Enemy->GetEnemyTier());
	}
}

UUDPlayerHUDWidget* const AUDPlayerCharacter::GetHUDWidget()
{
	return HUDWidget;
}

UUDRoundScreenWidget* const AUDPlayerCharacter::GetRoundScreenWidget()
{
	return RoundScreenWidget;
}

AUDPlayerController* const AUDPlayerCharacter::GetUDPlayerController() const
{
	return UDPlayerController;
}

void AUDPlayerCharacter::NotifyOnPlayerProjectileHit(AUDPlayerAttackProjectile* Projectile, AActor* ActorHit)
{
	if (OnPlayerProjectileHit.IsBound())
	{
		FPlayerProjectileHitData ProjectileHitData;
		ProjectileHitData.Projectile = Projectile;
		ProjectileHitData.ActorHit = ActorHit;

		OnPlayerProjectileHit.Broadcast(ProjectileHitData);
	}
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

void AUDPlayerCharacter::PressUIViaController()
{
	if (UDPlayerController->LinkedButton)
	{
		UDPlayerController->LinkedButton->OnPressed.Broadcast();
	}
}

void AUDPlayerCharacter::ReleaseUIViaController()
{
	if (UDPlayerController->LinkedButton)
	{
		UDPlayerController->LinkedButton->OnReleased.Broadcast();
		UDPlayerController->LinkedButton->OnClicked.Broadcast();
	}
}

void AUDPlayerCharacter::SetIsInMenu(bool bIsPaused)
{
	//Pause the game if the round screen is being opened. Unpause if it is being closed.
	UGameplayStatics::SetGamePaused(GetWorld(), bIsPaused);

	//Show/Hide the cursor based on whether or not the menu is being shown/hidden.
	UDPlayerController->bShowMouseCursor = bIsPaused;

	bInGameMenu = bIsPaused;

	if (bIsPaused)
	{
		//Swap the controller to the UI input mode. No more clicking the screen to get the UI to work. It's over. It's dead.
		UDPlayerController->SetInputMode(FInputModeGameAndUI());

		//Enable the UI controls
		InputSubsystem->AddMappingContext(GameMenuMappingContext, 0);

		//Disable the player controls
		InputSubsystem->RemoveMappingContext(PlayerCharacterMappingContext);
	}
	else
	{
		UDPlayerController->SetInputMode(FInputModeGameOnly());

		InputSubsystem->RemoveMappingContext(GameMenuMappingContext);

		//Reenable the player controls
		InputSubsystem->AddMappingContext(PlayerCharacterMappingContext, 0);
	}
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

UArrowComponent* AUDPlayerCharacter::GetAttackSpawnComponent()
{
	return AttackSpawnComponent;
}
