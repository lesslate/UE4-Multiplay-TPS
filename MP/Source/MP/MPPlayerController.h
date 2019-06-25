// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MP_API AMPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMPPlayerController();

	
protected:
	virtual void BeginPlay() override;
};
