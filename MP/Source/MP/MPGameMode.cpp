// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MPGameMode.h"
#include "MPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MPPlayerController.h"

AMPGameMode::AMPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/Character/BP_MPPlayer.BP_MPPlayer_C'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		PlayerControllerClass = AMPPlayerController::StaticClass();
	}
}

