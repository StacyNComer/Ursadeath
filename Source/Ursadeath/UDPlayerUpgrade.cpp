// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerUpgrade.h"


UUDPlayerUpgrade* UUDPlayerUpgrade::CreatePlayerUpgrade(TSubclassOf<UUDPlayerUpgrade> UpgradeClass, AUDPlayerCharacter* Player)
{
	UUDPlayerUpgrade* Upgrade = NewObject<UUDPlayerUpgrade>(UpgradeClass, UpgradeClass);

	Upgrade->OwningPlayer = Player;

	Upgrade->OnUpgradeAdded();

	return Upgrade;
}

FRewardInfo UUDPlayerUpgrade::GetRewardInfo()
{
	return RewardInfo;
}

