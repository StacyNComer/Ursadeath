// Fill out your copyright notice in the Description page of Project Settings.


#include "UrsadeathGameInstance.h"
#include "UDArena.h"
#include "UDPlayerCharacter.h"

void UUrsadeathGameInstance::StartWave(FEnemyWave Wave)
{
	PlayerCharacter->DisplayEnemyWave(Wave);
	GameArena->SetCurrentWave(Wave);
}