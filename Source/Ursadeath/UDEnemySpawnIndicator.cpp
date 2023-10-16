// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemySpawnIndicator.h"

void UUDEnemySpawnIndicator::SetEnemyCount(int Count)
{
	EnemyCount = Count;
	OnEnemyCountChanged(EnemyCount);
}

void UUDEnemySpawnIndicator::DecrementEnemyCount()
{
	SetEnemyCount(EnemyCount - 1);
}

void UUDEnemySpawnIndicator::NativeOnInitialized()
{
	SetEnemyCount(0);
}