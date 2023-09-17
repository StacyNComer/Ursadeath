// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerAbilityBase.h"
#include "UDPlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/ArrowComponent.h"

UUDPlayerAbilityBase::UUDPlayerAbilityBase()
{
	InputTrigger = ETriggerEvent::Triggered;
}

// Called when the game starts
void UUDPlayerAbilityBase::BeginPlay()
{
	Super::BeginPlay();

	//Set the owning player character.
	OwningPlayer = Cast<AUDPlayerCharacter>(GetOwner());
}


// Called every frame
void UUDPlayerAbilityBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UUDPlayerAbilityBase::TryUseAbility()
{
	if (CanUseAbility())
	{
		OwningPlayer->SpawnAttack(AttackActorClass);

		//Spawn the abilities particles, if they exist.
		if (ParticleSystem)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(ParticleSystem, OwningPlayer->GetAttackSpawnComponent(), FName(TEXT("None")), FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
		}

		NotifyOnAbilitySuccessful();

		return true;
	}
	else
	{
		return false;
	}
}

UInputAction* UUDPlayerAbilityBase::GetInputAction()
{
	return InputAction;
}

ETriggerEvent UUDPlayerAbilityBase::GetInputTrigger()
{
	return InputTrigger;
}

bool UUDPlayerAbilityBase::CanUseAbility()
{
	return true;
}

void UUDPlayerAbilityBase::NotifyOnAbilitySuccessful()
{
	//Broadcast OnAbilitySuccessful if it is bound.
	if (OnAbilitySuccessful.IsBound())
	{
		OnAbilitySuccessful.Broadcast();
	}
}

