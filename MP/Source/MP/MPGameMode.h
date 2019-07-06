// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE(FEndGame);

UCLASS(minimalapi)
class AMPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMPGameMode();

	FEndGame EndGame;

};



