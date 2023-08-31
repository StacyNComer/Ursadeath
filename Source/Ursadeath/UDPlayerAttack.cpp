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

void AUDPlayerAttack::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//Apply any attacks that this enemy overlaps
	AUDEnemy* EnemyHit = Cast<AUDEnemy>(OtherActor);
	if (EnemyHit != nullptr)
	{
		ApplyAttackToEnemy(EnemyHit, AttackStats);
		
		Cast<AUDPlayerCharacter>(Owner)->AddEnergy(EnergyGain);
	}
}

// Called every frame
void AUDPlayerAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUDPlayerAttack::ApplyAttackToEnemy(AUDEnemy* Enemy, const FPlayerAttackStats AttackStatsStruct)
{
	UUDPlayerAttackData* AttackData = UUDPlayerAttackData::CreatePlayerAttackData(AttackStats);

	Enemy->ReceiveAttack(AttackData);

	//Tell UE to delete the AttackData now that we are done with it.
	AttackData->ConditionalBeginDestroy();
}

