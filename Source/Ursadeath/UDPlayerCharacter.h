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
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** A component that represents the spawning position and rotation for attacks.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* AttackSpawnComponent;

	///////////////////////////////////////////////////////////////////////// Input Actions

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		FPlayerAbility PrimaryFireAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		FPlayerAbility RocketAbility;

protected:

	/** The time in seconds between shots of the player's primary fire.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking, meta = (AllowPrivateAccess = "true"))
		float primaryCooldown = .15f;

	/** The current remaining seconds until the player can fire their primary again.*/
	UPROPERTY(BlueprintReadOnly, Category = Attacking)
		float primaryCooldownTracker;

	/** Player Controller (cached so it doesn't need to be casted every time!) */
	APlayerController* playerController;

public:
	AUDPlayerCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float deltaTime);

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

protected:

	/** Spawns the given attack class rotated to where the player's camera is facing.*/
	void SpawnAttack(const TSubclassOf<AUDPlayerAttack> attackClass);

	///////////////////////////////////////////////////////////////////////// Input Functions
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Fires a primary attack, if it is off of cooldown. Used for Input*/
	void FirePrimary();

	/** Spawns the Rocket Ability. Kaboom! Used for Input.*/
	void FireRocket();

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

