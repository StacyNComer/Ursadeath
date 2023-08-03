// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerAttackData.h"

UUDPlayerAttackData* UUDPlayerAttackData::CreatePlayerAttackData(FPlayerAttackStats AttackStats)
{
	UUDPlayerAttackData* AttackData = NewObject<UUDPlayerAttackData>();

	AttackData->AttackStats = AttackStats;

	return AttackData;
}

void UUDPlayerAttackData::SetDamage(int32 value)
{
	AttackStats.Damage = value;
}

void UUDPlayerAttackData::SetStunTime(float value)
{
	AttackStats.StunTime = value;
}
