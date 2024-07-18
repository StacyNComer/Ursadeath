// Fill out your copyright notice in the Description page of Project Settings.


#include "UDEnemyDeathNotify.h"
#include <Kismet/GameplayStatics.h>

void UUDEnemyDeathNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);

	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"), true);
	MeshComp->SetSimulatePhysics(true);
}