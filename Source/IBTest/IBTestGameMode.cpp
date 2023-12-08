// Copyright Epic Games, Inc. All Rights Reserved.

#include "IBTestGameMode.h"
#include "IBTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIBTestGameMode::AIBTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
