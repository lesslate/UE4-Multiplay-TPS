// Fill out your copyright notice in the Description page of Project Settings.

#include "MPPlayerAnimInstance.h"
#include "MPPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ConstructorHelpers.h"
#include "Engine/Engine.h"



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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DEATH_MONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Stand/Falling_Forward_Death_Montage.Falling_Forward_Death_Montage'"));
	if (DEATH_MONTAGE.Succeeded())
	{
		DeathMontage = DEATH_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CROUCH_DEATHMONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Crouch/Crouch_Death_Montage.Crouch_Death_Montage'"));
	if (CROUCH_DEATHMONTAGE.Succeeded())
	{
		CrouchDeathMontage = CROUCH_DEATHMONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> PRONE_DEATHMONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Prone/Prone_Death_Montage.Prone_Death_Montage'"));
	if (PRONE_DEATHMONTAGE.Succeeded())
	{
		ProneDeathMontage = PRONE_DEATHMONTAGE.Object;
	}


	static ConstructorHelpers::FObjectFinder<UAnimMontage> RELOAD_MONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Stand/Reloading_Montage.Reloading_Montage'"));
	if (RELOAD_MONTAGE.Succeeded())
	{
		ReloadMontage = RELOAD_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CROUCH_RELOADMONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Crouch/Reload_Montage.Reload_Montage'"));
	if (CROUCH_RELOADMONTAGE.Succeeded())
	{
		CrouchReloadMontage = CROUCH_RELOADMONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> PRONE_RELOADMONTAGE(TEXT("AnimMontage'/Game/SoldierAnimation/Prone/Prone_Reloading_Montage.Prone_Reloading_Montage'"));
	if (PRONE_RELOADMONTAGE.Succeeded())
	{
		ProneReloadMontage = PRONE_RELOADMONTAGE.Object;
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

void UMPPlayerAnimInstance::PlayDeathMontage()
{
	CHECK(DeathMontage != nullptr)
	Montage_Play(DeathMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayCrouchDeathMontage()
{
	CHECK(CrouchDeathMontage != nullptr)
	Montage_Play(CrouchDeathMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayProneDeathMontage()
{
	CHECK(ProneDeathMontage != nullptr)
	Montage_Play(ProneDeathMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayReloadMontage()
{
	CHECK(ReloadMontage != nullptr)
	Montage_Play(ReloadMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayCrouchReloadMontage()
{
	CHECK(CrouchReloadMontage != nullptr)
	Montage_Play(CrouchReloadMontage, 1.0f);
}

void UMPPlayerAnimInstance::PlayProneReloadMontage()
{
	CHECK(ProneReloadMontage != nullptr)
	Montage_Play(ProneReloadMontage, 1.0f);
}

void UMPPlayerAnimInstance::AnimNotify_ReloadEnd()
{
	MPPlayer = Cast<AMPPlayer>(TryGetPawnOwner());
	MPPlayer->ReloadEnd();
	
}

void UMPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	auto Player = Cast<AMPPlayer>(TryGetPawnOwner());
	
	if (::IsValid(Player))
	{
		CurrentPawnSpeed = Player->GetVelocity().Size();
		IsInAir = Player->GetMovementComponent()->IsFalling();
		IsCrouch = Player->IsCrouch;
		IsProne = Player->IsProne;
		IsAiming = Player->IsAiming;
		ControllerPitch = Player->WraistPitch;
		IsReload = Player->IsReloading;

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
