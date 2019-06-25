// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	bool IsCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	bool IsProne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	float ControllerPitch;
};
