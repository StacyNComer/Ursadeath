// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerAttack.h"
#include "UDPlayerAttackData.h"

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

UUDPlayerAttackData* AUDPlayerAttack::CreateAttackDataFrom()
{
	return UUDPlayerAttackData::CreatePlayerAttackData(AttackStats);
}

