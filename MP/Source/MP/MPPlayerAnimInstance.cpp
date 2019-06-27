// Fill out your copyright notice in the Description page of Project Settings.

#include "MPPlayerAnimInstance.h"
#include "MPPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ConstructorHelpers.h"



UMPPlayerAnimInstance::UMPPlayerAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> FIRE_MONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Stand/Firing_Rifle_Montage.Firing_Rifle_Montage'"));
	if (FIRE_MONTAGE.Succeeded())
	{
		FireMontage = FIRE_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CROUCH_FIREMONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Crouch/Crouch_Fire_Montage.Crouch_Fire_Montage'"));
	if (CROUCH_FIREMONTAGE.Succeeded())
	{
		CrouchFireMontage = CROUCH_FIREMONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> PRONE_FIREMONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Prone/Prone_Firing_Rifle_Montage.Prone_Firing_Rifle_Montage'"));
	if (PRONE_FIREMONTAGE.Succeeded())
	{
		ProneFireMontage = PRONE_FIREMONTAGE.Object;
	}


	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsCrouch = false;
	IsProne = false;
}

void UMPPlayerAnimInstance::NativeBeginPlay()
{

}

void UMPPlayerAnimInstance::PlayFireMontage()
{
	CHECK(FireMontage!=nullptr)
	Montage_Play(FireMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayCrouchFire()
{
	CHECK(CrouchFireMontage != nullptr)
	Montage_Play(CrouchFireMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayProneFire()
{
	CHECK(ProneFireMontage != nullptr)
	Montage_Play(ProneFireMontage, 1.0f);
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
		ControllerPitch = Player->WraistPitch;

		if (IsAiming)
		{
			ControllerPitch = Player->WraistPitch;
		}
		else
		{
			ControllerPitch = 0.0f;
		}
	}


}
