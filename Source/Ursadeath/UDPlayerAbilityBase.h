// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UDPlayerAbilityBase.generated.h"

class AUDPlayerAttack;
class AUDPlayerCharacter;
class UNiagaraSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilitySuccessful);

/** The base class for components meant to represent player abilities. This component assumes that it is attached to a UDPlayerCharacter. If it somehow gets attached to something else, prepare for crashes!*/
UCLASS(Abstract, ClassGroup=(Player))
class URSADEATH_API UUDPlayerAbilityBase : public UActorComponent 
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		ETriggerEvent InputTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
		TSubclassOf<AUDPlayerAttack> AttackActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<UNiagaraSystem> ParticleSystem;

	/** The audio played whenever the ability is used successfully. Safe to omit.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
		TObjectPtr<USoundBase> AbilitySound;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerCharacter> OwningPlayer;

public:	

	UPROPERTY(BlueprintReadWrite)
	/** A delegate broadcast when this ability is used successfully. Use to create additional functionality beyond spawning the represenative attack.*/
	FAbilitySuccessful OnAbilitySuccessful;

	/** If CanUseAbility returns true, uses the ability by spawning the attack associated with the ability and calling NotifyOnAbilitySuccessful. Returns true if the ability use is successful.*/
	UFUNCTION(BlueprintCallable)
	bool TryUseAbility();

	/** Returns the input action this ability is bound to.*/
	UInputAction* GetInputAction();

	/** Returned the Trigger Event the input bound to this actor should have.*/
	ETriggerEvent GetInputTrigger();

protected:
	UUDPlayerAbilityBase();

	// Called when the game starts
	virtual void BeginPlay() override;

	/** If this returns true, the player will be able to use this ability.*/
	virtual bool CanUseAbility();

	/** Broadcasts the OnAbilitySuccessful delegate. Can be overriden in derived classes for additional functionality when an ability is used successfully.*/
	virtual void NotifyOnAbilitySuccessful();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
