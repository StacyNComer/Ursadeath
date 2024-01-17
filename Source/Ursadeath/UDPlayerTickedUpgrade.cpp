// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerTickedUpgrade.h"
#include "UDPlayerCharacter.h"

void UUDPlayerTickedUpgrade::AttachToPlayer(AUDPlayerCharacter* Player)
{
	Player->AddTickedUpgrade(this);
}