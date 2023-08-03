// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrsadeathGameMode.h"
#include "UrsadeathCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUrsadeathGameMode::AUrsadeathGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Ursadeath/PlayerCharacter/Blueprints/BP_UrsadeathCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
