// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPlayerUpgrade.h"
#include "UDPlayerCharacter.h"

UUDPlayerUpgrade* UUDPlayerUpgrade::CreatePlayerUpgrade(TSubclassOf<UUDPlayerUpgrade> UpgradeClass, AUDPlayerCharacter* Player)
{
	UUDPlayerUpgrade* Upgrade = NewObject<UUDPlayerUpgrade>(Player, UpgradeClass);

	Upgrade->OwningPlayer = Player;

	Upgrade->AttachToPlayer(Player);

	Upgrade->OnUpgradeAdded(Player);

	return Upgrade;
}

UWorld* UUDPlayerUpgrade::GetWorld() const
{
	/*
	 (This is taken from the AActor source code. The most important part is checking that the instance is not a CDO) 
	 CDO objects do not belong to a world
	 If the actors outer is destroyed or unreachable we are shutting down and the world should be nullptr
	*/
	if (!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		return GetOuter()->GetWorld();
	}

	return nullptr;
}

void UUDPlayerUpgrade::AttachToPlayer(AUDPlayerCharacter* Player)
{
	Player->AddUpgrade(this);
}


