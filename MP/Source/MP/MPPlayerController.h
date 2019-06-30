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

	UPROPERTY(EditDefaultsOnly, Category = Scope)
	TSubclassOf<class UWidget_HUD> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI)
	class UWidget_HUD* HUDWidget;

	
protected:
	virtual void BeginPlay() override;
};
