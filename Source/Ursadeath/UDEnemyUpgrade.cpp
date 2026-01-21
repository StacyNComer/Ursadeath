// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemyUpgrade.h"
#include "UrsadeathGameInstance.h"

UUDEnemyUpgrade* UUDEnemyUpgrade::CreateUpgrade(TSubclassOf<UUDEnemyUpgrade> UpgradeClass, UObject* Outer)
{
	UUDEnemyUpgrade* Upgrade = NewObject<UUDEnemyUpgrade>(Outer, UpgradeClass);

	return Upgrade;
}

/*void UUDEnemyUpgrade::BeginDestroy()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, GetClass()->GetName());

	Super::BeginDestroy();
}*/


UWorld* UUDEnemyUpgrade::GetWorld() const
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

FEnemySpawnData UUDEnemyUpgrade::OnEnemyDataChangeApplied_Implementation(FEnemySpawnData OriginalData) const
{
	return OriginalData;
}