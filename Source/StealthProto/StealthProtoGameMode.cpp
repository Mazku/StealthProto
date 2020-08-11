// Copyright Epic Games, Inc. All Rights Reserved.

#include "StealthProtoGameMode.h"
#include "StealthProtoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AStealthProtoGameMode::AStealthProtoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
