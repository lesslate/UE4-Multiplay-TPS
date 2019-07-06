// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UWidget_Death> WidgetDeathClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UWidget_Winner> WidgetWinnerClass;

	UFUNCTION(client, Reliable,BlueprintCallable, Category = "Widget")
	void AddDeathWidget();
	void AddDeathWidget_Implementation();

	UFUNCTION(client, Reliable,BlueprintCallable, Category = "Widget")
	void AddWinnerWidget();
	void AddWinnerWidget_Implementation();

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI)
	class UWidget_Death* DeathWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI)
	class UWidget_Winner* WinnerWidget;

};
