// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_NetworkDemoGameMode.h"
#include "UE_NetworkDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE_NetworkDemoGameMode::AUE_NetworkDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
