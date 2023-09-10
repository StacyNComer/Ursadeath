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
class UUDPlayerCooldownAbility;
class UUDPlayerEnergyAbility;

/** A structure holding the common data for the player's abilities. Non-common data, such as if the attack has a cooldown or input functions, is stored in the PlayerCharacter itself.*/
USTRUCT(BlueprintType)
struct FPlayerAbility
{
	 GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attacking)
	TSubclassOf<AUDPlayerAttack> AttackActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attacking)
	class UInputAction* InputAction;
};

UCLASS(config = Game)
class AUDPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

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

	/** The player's HUD. */
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UUDPlayerHUDWidget> PlayerHUDWidget;

	/** The player controller casted to UDPlayerController. */
	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerController> UDPlayerController;

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

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
		bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool GetHasRifle();

	UFUNCTION(BlueprintCallable, Category = Status)
		/** Adds to the player's current store of energy, though not above the player's Max Energy.*/
		void AddEnergy(float Value);

	/** Spawns the given attack class rotated to where the player's camera is facing.*/
	void SpawnAttack(const TSubclassOf<AUDPlayerAttack> attackClass);

	float GetEnergy();

	/** Expends the given amount of player energy and updates the UI. This does not keep the value from dropping below zero, as it is assumed the value will be tested beforehand.*/
	void ExpendEnergy(float ToExpend);

protected:

	///////////////////////////////////////////////////////////////////////// Input Functions
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

