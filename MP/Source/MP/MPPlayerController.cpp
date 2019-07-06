// Fill out your copyright notice in the Description page of Project Settings.

#include "MPPlayerController.h"
#include "ConstructorHelpers.h"
#include "Widget_HUD.h"
#include "Widget_Death.h"
#include "Widget_Winner.h"
#include "MPGameMode.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AMPPlayerController::AMPPlayerController()
{
	static ConstructorHelpers::FClassFinder<UWidget_Death> DEATHWIDGET(TEXT("WidgetBlueprint'/Game/Widget/BP_DeathWidget.BP_DeathWidget_C'"));
	if (DEATHWIDGET.Succeeded())
	{
		WidgetDeathClass = DEATHWIDGET.Class;
	}

	static ConstructorHelpers::FClassFinder<UWidget_Winner> WINNERWIDGET(TEXT("WidgetBlueprint'/Game/Widget/BP_WinnerWidget.BP_WinnerWidget_C'"));
	if (WINNERWIDGET.Succeeded())
	{
		WidgetWinnerClass = WINNERWIDGET.Class;
	}

}

void AMPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMPPlayerController::AddDeathWidget_Implementation()
{
	DeathWidget = CreateWidget<UWidget_Death>(this, WidgetDeathClass);
	CHECK(nullptr != DeathWidget);
	DeathWidget->AddToViewport();
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
	
}

void AMPPlayerController::AddWinnerWidget_Implementation()
{
	WinnerWidget = CreateWidget<UWidget_Winner>(this, WidgetWinnerClass);
	CHECK(nullptr != WinnerWidget);
	WinnerWidget->AddToViewport();
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}


