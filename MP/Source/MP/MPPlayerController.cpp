// Fill out your copyright notice in the Description page of Project Settings.

#include "MPPlayerController.h"
#include "ConstructorHelpers.h"
#include "Widget_HUD.h"

AMPPlayerController::AMPPlayerController()
{
	static ConstructorHelpers::FClassFinder<UWidget_HUD> HUDWIDGETC(TEXT("WidgetBlueprint'/Game/Widget/BP_Widget_HUD.BP_Widget_HUD_C'"));
	if (HUDWIDGETC.Succeeded())
	{
		HUDWidgetClass = HUDWIDGETC.Class;
	}
}


void AMPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()&&HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UWidget_HUD>(this, HUDWidgetClass);
		HUDWidget->AddToViewport();
	}
}

