// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemy.h"
#include "UDPlayerAttack.h"
#include "UDPlayerAttackData.h"

// Sets default values
AUDEnemy::AUDEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUDEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AUDEnemy::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//Apply any attacks that this enemy overlaps
	AUDPlayerAttack* PlayerAttack = Cast<AUDPlayerAttack>(OtherActor);
	if (PlayerAttack != nullptr)
	{
		UUDPlayerAttackData* AttackData = PlayerAttack->CreateAttackDataFrom();

		ReceiveAttack(AttackData);

		//Tell UE to delete the AttackData now that we are done with it.
		AttackData->ConditionalBeginDestroy();
	}
}

// Called every frame
void AUDEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUDEnemy::ReceiveAttack(UUDPlayerAttackData* AttackData)
{
	if(OnAttackRecieved.IsBound())
	{
		OnAttackRecieved.Broadcast(AttackData);
	}

	health -= AttackData->AttackStats.Damage;

	if (!bUndieable && health <= 0)
	{
		Destroy();
	}
}

