// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrsadeathGameMode.h"
#include "UDPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUrsadeathGameMode::AUrsadeathGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Ursadeath/PlayerCharacter/Blueprints/BP_UrsadeathPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
