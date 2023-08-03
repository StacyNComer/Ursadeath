// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UDEnemy.generated.h"

class UUDPlayerAttackData;

/*A delegate type for when the enemy is damaged.*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackReceived, UUDPlayerAttackData*, DamageData);

UCLASS()
class URSADEATH_API AUDEnemy : public APawn
{
	GENERATED_BODY()

public:
	/** A delegate invoked before an enemy is affected by an attack during ReceiveAttack. This delegate is used to modify the attack based on an enemy's strength/weakness toward it.*/
	UPROPERTY(BlueprintAssignable)
	FAttackReceived OnAttackRecieved;

	/** If true, the enemy is immune to dying.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUndieable;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
	int32 health;

public:
	// Sets default values for this pawn's properties
	AUDEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when something overlaps any of this actor's components.
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Applies the given attack data to this enemy. The attacking player's OnAttackHit and this actor's own OnAttackReceived is called before the damage/stun is applied to the enemy.*/
	UFUNCTION(BlueprintCallable, Category=Status)
	void ReceiveAttack(UUDPlayerAttackData* AttackData);
};
