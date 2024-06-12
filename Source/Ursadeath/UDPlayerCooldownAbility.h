// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPlayerAbilityBase.h"
#include "UDPlayerCooldownAbility.generated.h"

/**
 * A component class for player abilities with cooldown/fire rates.
 */
UCLASS()
class URSADEATH_API UUDPlayerCooldownAbility : public UUDPlayerAbilityBase
{
	GENERATED_BODY()

protected:
	/** The remaining time on the ability's cooldown.*/
	UPROPERTY(BlueprintReadOnly)
		float CurrentCooldown;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Attack)
		float Cooldown;

protected:
	UUDPlayerCooldownAbility();

	bool CanUseAbility() override;

	void NotifyOnAbilitySuccessful() override;

public:
	/** Force the ability to be on cooldown for the set amount of time. Note that this will override whatever the ability's remaining cooldown currently is.*/
	UFUNCTION(BlueprintCallable)
	void ForceCooldown(float CooldownTime);

public:
	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
