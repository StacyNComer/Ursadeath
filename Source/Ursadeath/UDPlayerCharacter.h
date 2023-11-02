// Copyright Epic Games, Inc. All Rights Reserved.

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
class UUDPlayerCooldownAbility;
class UUDPlayerEnergyAbility;
class AUDEnemy;
struct FEnemyWave;
enum class EEnemyTier;

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
};

UCLASS(Abstract, config = Game)
class AUDPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKillSignature, FEnemyHitData, HitData);

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* RoundMenuMappingContext;

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

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		FPlayerAbility PrimaryFireAbility;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerCooldownAbility> PrimaryFireAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerCooldownAbility> MeleeAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerEnergyAbility> RocketAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UUDPlayerEnergyAbility> ShockwaveAbility;

protected:
	UPROPERTY(BlueprintAssignable)
		FOnEnemyKillSignature OnEnemyDeath;

	/** The max amount of health a player may have.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float MaxHealth;

	/** How much health the player currently has.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float CurrentHealth;

	/** The max amount of energy a player may store.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		float MaxEnergy;

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

	/** The player's HUD.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDPlayerHUDWidget> PlayerHUDWidget;

	/** The player's Round Screen for viewing the upcoming enemy waves and their upgrades.*/
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDRoundScreenWidget> RoundScreenWidget;

	/** The player controller casted to UDPlayerController. */
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerController> UDPlayerController;

	/** If true, energy is never expended and the player is always considered to have max energy (this won't change the UI to reflect this)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
		bool bInfiniteEnergy;

public:
	AUDPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

public:
	/** A global method for getting the current active player character.*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static AUDPlayerCharacter* GetCharacterInPlay(UObject* WorldContextObject);

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RoundMenuAction;

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

	/** Trigger the effects of a player using a health pickup. Additional effects beyond the default healing can be bound using the OnHealthPickupUsed delegate.*/
	void NotifyOnHealthPickupUsed();

	/** Spawns the given attack class rotated to where the player's camera is facing.*/
	void SpawnAttack(const TSubclassOf<AUDPlayerAttack> AttackClass);

	float GetEnergy();

	/** Sets the player's current energy to the given value and updates the player's UI. All methods that modify the current energy in some way must call this method to update the UI.*/
	void SetEnergy(float Value);

	/** Adds to the player's current store of energy, though not above the player's Max Energy.*/
	void AddEnergy(float ToAdd);
	
	/** Expends the given amount of player energy and updates the UI. This does not keep the value from dropping below zero, as it is assumed the value will be tested beforehand.*/
	void ExpendEnergy(float ToExpend);

	/** Sets the player's current health to the given value and update's their UI. All methods that modify the current health in some what must call this method to update the UI.*/
	void SetHealth(int Value);

	/** Restore the given amount of helath to the player, but not beyond their max health.*/
	void RestoreHealth(int Value);

	/** Reports that the given enemy has been killed by the given attack to the player, updating the player's UI and invoking the OnEnemyKill delegate.*/
	void NotifyOnEnemyKill(AUDEnemy* EnemyKilled, AUDPlayerAttack* Attack);

	/** Displays the given enemy wave on the player's UI.*/
	void DisplayEnemyWave(FEnemyWave Wave);

protected:

	///////////////////////////////////////////////////////////////////////// Input Functions
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Swaps whether the round menu is on or off.*/
	void ToggleRoundMenu();

	/** Uses the primary fire ability. Used for Input.*/
	void UsePrimaryAbility();

	/** Uses the melee attack ability. Used for Input.*/
	void UseMeleeAbility();

	/** Uses the rocket ability. Used for Input.*/
	void UseRocketAbility();

	/** Uses the shockwave ability. Used for Input.*/
	void UseShockwaveAbility();

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

