// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
#include "Animation/AnimInstance.h"
#include "MPPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MP_API UMPPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMPPlayerAnimInstance();


	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

	void PlayFireMontage();
	void PlayCrouchFire();
	void PlayProneFire();

	void PlayDeathMontage();
	void PlayCrouchDeathMontage();
	void PlayProneDeathMontage();

	void PlayReloadMontage();
	void PlayCrouchReloadMontage();
	void PlayProneReloadMontage();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	bool IsReload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	bool IsCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	bool IsProne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	float ControllerPitch;

	UFUNCTION()
	void AnimNotify_ReloadEnd();




private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* FireMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* CrouchFireMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ProneFireMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* CrouchDeathMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ProneDeathMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* CrouchReloadMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ProneReloadMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	class AMPPlayer* MPPlayer;
};
