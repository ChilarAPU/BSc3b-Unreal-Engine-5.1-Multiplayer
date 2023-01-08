// Copyright Epic Games, Inc. All Rights Reserved.

#include "BSc3bGameMode.h"
#include "BSc3bCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABSc3bGameMode::ABSc3bGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
