// Fill out your copyright notice in the Description page of Project Settings.

#include "MPPlayerAnimInstance.h"
#include "MPPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ConstructorHelpers.h"



UMPPlayerAnimInstance::UMPPlayerAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsCrouch = false;
	IsProne = false;
}

void UMPPlayerAnimInstance::NativeBeginPlay()
{

}


void UMPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	auto Player = Cast<AMPPlayer>(TryGetPawnOwner());
	
	if (::IsValid(Player))
	{
		CurrentPawnSpeed = Player->GetVelocity().Size();
		IsInAir = Player->GetMovementComponent()->IsFalling();
		IsCrouch = Player->bIsCrouched;
		IsProne = Player->IsProne;
		IsAiming = Player->IsAiming;
	}

}
