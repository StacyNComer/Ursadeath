
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "UDPlayerCharacter.generated.h"

class UDPlayerDamageData;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UArrowComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class AUDPlayerAttack;
class AUDPlayerController;
class UUDPlayerHUDWidget;
class UUDRoundScreenWidget;
class UUDGameOverWidget;
class UUDPlayerCooldownAbility;
class UUDPlayerEnergyAbility;
class AUDEnemy;
class UUrsadeathGameInstance;
class UEnhancedInputLocalPlayerSubsystem;
class UUDPlayerUpgrade;
class UUDPlayerTickedUpgrade;
class AUDHealthPickup;
class AUDPlayerAttackProjectile;
class UUDPlayerStatusIcon;
class UUDPauseMenuWidget;
struct FEnemyWave;
enum class EEnemyTier : uint8;

/** Holds the data for events ocurring when an attack hits an enemy.*/
USTRUCT(BlueprintType)
struct FEnemyHitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<AUDEnemy> Enemy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EEnemyTier EnemyTier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<AUDPlayerAttack> Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool EnemyWasKilled;
};

USTRUCT(BlueprintType)
struct FPlayerProjectileHitData
{
	GENERATED_BODY()  

	/** The projectile involved in the interaction.*/
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AUDPlayerAttackProjectile> Projectile;

	/** The actor that the projectile has hit. This is not necessarily an enemy.*/
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ActorHit;
};

UCLASS(Abstract, config = Game)
class AUDPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKillSignature, FEnemyHitData, HitData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackSpawnedSignature, AUDPlayerAttack*, AttackActor);

	/** Called when a projectile hits something but before any secondary effects, such as explosions, occur.*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerProjectileHitSignature, FPlayerProjectileHitData, ProjectileHitData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthPickupUsedSignature, AUDHealthPickup*, HealthPickup);

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;

	/** MappingContexts */
	/** The mapping context for the player character outside of menus.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* PlayerCharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* GameMenuMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* RoundMenuMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* PauseMenuMappingContext;

	/** A component that represents the spawning position and rotation for attacks.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UArrowComponent> AttackSpawnComponent;

	///////////////////////////////////////////////////////////////////////// Input Actions

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Controller Click Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ControllerClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* UIBackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerCooldownAbility> PrimaryFireAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerCooldownAbility> MeleeAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerEnergyAbility> RocketAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerEnergyAbility> ShockwaveAbility;

	/** Holds the player's upgrades so the Garbage Collection doesn't abduct them.*/
	UPROPERTY()
		TArray<TObjectPtr<UUDPlayerUpgrade>> Upgrades;

	UPROPERTY()
		TArray<TObjectPtr<UUDPlayerTickedUpgrade>> TickedUpgrades;

	/** The time until the player ceases to be hasted.*/
	float HasteTimeTracker = 0;

	/** The status icon that shows when the player is hasted.*/
	TObjectPtr<UUDPlayerStatusIcon> HasteStatusWidget;

	/** The status icon that shows when the player is invulnerable.*/
	TObjectPtr<UUDPlayerStatusIcon> InvulnerableStatusWidget;
	
protected:
	UPROPERTY(BlueprintAssignable)
		FOnEnemyKillSignature OnEnemyHit;

	UPROPERTY(BlueprintAssignable)
		FOnAttackSpawnedSignature OnAttackSpawned;

	UPROPERTY(BlueprintAssignable)
		FPlayerProjectileHitSignature OnPlayerProjectileHit;

	UPROPERTY(BlueprintAssignable)
		FOnHealthPickupUsedSignature OnHealthPickupUsed;

	/** The max amount of health a player may have.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float MaxHealth;

	/** How much health the player currently has.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float CurrentHealth;

	/** The max amount of energy a player may store.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float MaxEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float HasteSpeedMultiplier;

	/** The current energy the player has stored. Use SetCurrentEnergy to modifiy this value.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float CurrentEnergy;

	/** When the player performs a melee attack, their primary fire is put on cooldown for this amount of time.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		float MeleePrimaryPause;

	/** The class the player's HUD will be created as.*/
	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<UUDPlayerHUDWidget> PlayerHUDWidgetClass;

	/** The class the player's Round Screen will be created as.*/
	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<UUDRoundScreenWidget> RoundScreenWidgetClass;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<UUDPauseMenuWidget> PauseMenuWidgetClass;

	/**The class the game over screen will be created as.*/
	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<UUDGameOverWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, Category = UI)
		UTexture2D* HasteStatusImage;

	UPROPERTY(EditAnywhere, Category = UI)
		UTexture2D* InvulnerableStatusImage;

	/** The player's HUD.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDPlayerHUDWidget> HUDWidget;

	/** The player's Round Screen for viewing the upcoming enemy waves and their upgrades.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDRoundScreenWidget> RoundScreenWidget;

	/** The widget that appears when the player completes the game. Or dies.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDGameOverWidget> GameOverWidget;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDPauseMenuWidget> PauseMenuWidget;

	/** The player controller casted to UDPlayerController.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerController> UDPlayerController;

	/** Audio played when the player attempts to use an ability but does not have enough energy.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		TObjectPtr<USoundBase> NoEnergySound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		TObjectPtr<USoundBase> EnergyBarGainedSound;

	/** The audio that plays when the player takes damage.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		TObjectPtr<USoundBase> DamageSound;

	/** If true, energy is never expended and the player is always considered to have max energy (this won't change the UI to reflect this)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool bInfiniteEnergy;

	/** The player is immune to damage while this is true.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool bPermanentInvulnerability;

	/** How many seconds of invulnerability the player has left.*/
	float InvulnerabilityTimeTracker = 0;

	/** True if the player is in a menu and no other menues should be allowed to open.*/
	bool bInGameMenu;

	TObjectPtr<UUrsadeathGameInstance> UrsadeathGameInstance;

public:
	AUDPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

public:
	/** A global method for getting the current active player character.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static AUDPlayerCharacter* GetCharacterInPlay(UObject* WorldContextObject);

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RoundMenuAction;

	/** Pause Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* PauseAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
		bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool GetHasRifle();

	/** Returns the transform of the component ths player's attacks spawn at. */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		UArrowComponent* GetAttackSpawnComponent();

	/** Causes the player to lose the given amount of health. This does not keep the player's health from going below 0 because screwing up and seeing "-999 Health" is funny.*/
	UFUNCTION(BlueprintCallable)
		void DamagePlayer(int Damage);

	/** Restore the given amount of health to the player, but not beyond their max health.*/
	UFUNCTION(BlueprintCallable)
		void RestoreHealth(int Value);

	/** Gives the player the "Hasted" effect, increasing their movement speed for the given amount of seconds.*/
	UFUNCTION(BlueprintCallable)
		void ApplyHaste(float Time);

	/** Makes the player immune to any and all damage for the given amount of seconds.*/
	UFUNCTION(BlueprintCallable)
		void ApplyInvulnerability(float Time);

	/** Returns true while the player is invulnerable and shouldn't be effected by damage.*/
	UFUNCTION(BlueprintCallable)
		bool IsInvulnerable();

	/** Spawns the given attack with the player as the owner.*/
	UFUNCTION(BlueprintCallable)
		void SpawnAttack(const TSubclassOf<AUDPlayerAttack> AttackClass, FVector Location, FRotator Rotation);

	/** Spawns the given attack class rotated to where the player's camera is facing.*/
	UFUNCTION(BlueprintCallable)
		void SpawnAttackFromPlayer(const TSubclassOf<AUDPlayerAttack> AttackClass);

	void AddUpgrade(UUDPlayerUpgrade* Upgrade);

	void AddTickedUpgrade(UUDPlayerTickedUpgrade* Upgrade);

	/** Trigger the effects of a player using a health pickup. Additional effects beyond the default healing can be bound using the OnHealthPickupUsed delegate.*/
	void NotifyOnHealthPickupUsed(AUDHealthPickup* HealthPickup);

	/** Returns the player's current energy amount.*/
	float GetCurrentEnergy();

	/** Tests if the player has the given amount of energy or greater. Abilities should use TestEnergyWithFX so that FX play if the player doesn't have enough energy.*/
	bool TestEnergy(float amount);

	/** Tests if the player has the given amount of energy or greater, playing FX to the player if their energy amount is insufficient.*/
	bool TestEnergyWithFX(float amount);

	/** Sets the player's current energy to the given value and updates the player's UI. All methods that modify the current energy in some way must call this method to update the UI.*/
	void SetEnergy(float Value);

	/** Adds to the player's current store of energy, though not above the player's Max Energy.*/
	void AddEnergy(float ToAdd);
	
	/** Expends the given amount of player energy and updates the UI. This does not keep the value from dropping below zero, as it is assumed the value will be tested beforehand.*/
	void ExpendEnergy(float ToExpend);

	/** Sets the player's current health to the given value and update's their UI. All methods that modify the current health in some what must call this method to update the UI.*/
	void SetHealth(int Value);

	/** Reports that the given enemy has been hit by the given attack to the player, invoking the OnEnemyHit delegate and updating the player's UI if the enemy was killed.*/
	void NotifyOnEnemyHit(AUDEnemy* Enemy, AUDPlayerAttack* Attack, bool EnemyWasKilled);

	/** Returns a reference to the player's HUD wudget.*/
	UUDPlayerHUDWidget* const GetHUDWidget();

	/** Returns a reference to the player's round screen*/
	UUDRoundScreenWidget* const GetRoundScreenWidget();

	AUDPlayerController* const GetUDPlayerController() const;

	void NotifyOnPlayerProjectileHit(AUDPlayerAttackProjectile* Projectile, AActor* ActorHit);

	///////////////////////////////////////////////////////////////////////// Input Functions

	/** Swaps whether the round menu is on or off.*/
	void ToggleRoundMenu();

	/** Swaps whether the Pause menu is on or off*/
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	/** Regresses the pause menu toward its root "main" menu, closing the pause menu if it is already there.*/
	void BackOutPauseMenu();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Uses the primary fire ability. Used for Input.*/
	void UsePrimaryAbility();

	/** Uses the melee attack ability. Used for Input.*/
	void UseMeleeAbility();

	/** Uses the rocket ability. Used for Input.*/
	void UseRocketAbility();

	/** Uses the shockwave ability. Used for Input.*/
	void UseShockwaveAbility();

	/** If a Controller Button is linked to the Player Controller, call the button's click function.*/
	void PressUIViaController();

	void ReleaseUIViaController();

	//End Input Functions

	/** Pause the game and set the player input to work with menus. This can be used for any menu that should pause the game while the player interacts with it (e.g. the round screen)*/
	void SetIsInMenu(bool bIsPaused);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

