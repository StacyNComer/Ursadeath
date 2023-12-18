// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerAttack.h"
#include "UDEnemy.h"
#include "UDPlayerAttackData.h"
#include "UDPlayerCharacter.h"

// Sets default values
AUDPlayerAttack::AUDPlayerAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUDPlayerAttack::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUDPlayerAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUDPlayerAttack::ApplyAttackToEnemy(AUDEnemy* Enemy, const FPlayerAttackStats AttackStatsStruct)
{
	UUDPlayerAttackData* AttackData = UUDPlayerAttackData::CreatePlayerAttackData(AttackStatsStruct);

	Enemy->ReceiveAttack(AttackData, this);

	//Tell UE to delete the AttackData now that we are done with it.
	AttackData->ConditionalBeginDestroy();

	GetOwningPlayer()->AddEnergy(EnergyGain);
}

AUDPlayerCharacter* const AUDPlayerAttack::GetOwningPlayer()
{
	//Set the owning player if it hasn't been set yet. 
	//We set the ownng player here instead of in BeginPlay because the C++ begin play runs after blueprints; If the projectile hits an enemy the frame it spawns, OwningPlayer wouldn't have been set yet!
	if (!OwningPlayer)
	{
		OwningPlayer = CastChecked<AUDPlayerCharacter>(Owner);
	}

	return OwningPlayer;
}

EPlayerAttackType AUDPlayerAttack::GetAttackType()
{
	return AttackStats.AttackType;
}

