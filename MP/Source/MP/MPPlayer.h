// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
#include "GameFramework/Character.h"
#include "MPPlayer.generated.h"

UCLASS()
class MP_API AMPPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMPPlayer();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void AddControllerPitchInput(float Val);
	void AddControllerYawInput(float Val);

	void SetCrouchMovement();
	void SetProneMovement();

	UPROPERTY(EditDefaultsOnly, Category = Scope)
	TSubclassOf<class UScopeWidget> ScopeWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
	TSubclassOf<class AActor> AmmoClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI)
	class UScopeWidget* ScopeWidget;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void AddScopeWidget();

	UPROPERTY()
	FTimerHandle timer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class AMPPlayerController* MPPC;

	UPROPERTY(replicated,VisibleAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool IsSprint;

	UPROPERTY(replicated,VisibleAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool IsCrouch;

	UPROPERTY(replicated,VisibleAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool IsProne;

	UPROPERTY(replicated,VisibleAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool IsAiming;

	UPROPERTY(replicated, VisibleAnywhere, BlueprintReadWrite, Category = PlayerState)
	float WraistPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerState)
	float PreviousWraistPitch;

	UPROPERTY()
	class UGameplayStatics* GameStatic;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// 스프링암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* TPSpringArm;

	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FPCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* WeaponCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USphereComponent* Sphere;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	class USoundCue* ShotCue;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	class USoundCue* AimCue;

	// 무기 메시 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StaticMesh)
	class UStaticMeshComponent* WeaponMesh;

	// 오디어 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
	class UAudioComponent* PlayerAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking")
	float SprintSpeedMultiplier;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	//////////     Sprint   //////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void StartSprintServer();
	void StartSprintServer_Implementation();
	bool StartSprintServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void StartSprintMulticast();
	void StartSprintMulticast_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	void StopSprintServer();
	void StopSprintServer_Implementation();
	bool StopSprintServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void StopSprintMulticast();
	void StopSprintMulticast_Implementation();

	//////////  Crouch ////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void CrouchServer();
	void CrouchServer_Implementation();
	bool CrouchServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void CrouchMulticast();
	void CrouchMulticast_Implementation();

	/////// Prone //////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void ProneServer();
	void ProneServer_Implementation();
	bool ProneServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void ProneMulticast();
	void ProneMulticast_Implementation();

	/////// Aiming //////////////////////
	UFUNCTION()
	void Aiming();

	UFUNCTION(Reliable, Server, WithValidation)
	void AimingServer(bool Aiming);
	void AimingServer_Implementation(bool Aiming);
	bool AimingServer_Validate(bool Aiming);

	
	/////// Fire //////////////////////
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void OnFire();

	UFUNCTION()
	void FireWeapon(FTransform trans);

	UFUNCTION(Reliable, Server, WithValidation)
	void FireWeaponServer(TSubclassOf<AActor> Ammo,FTransform trans);
	void FireWeaponServer_Implementation(TSubclassOf<AActor> Ammo, FTransform trans);
	bool FireWeaponServer_Validate(TSubclassOf<AActor> Ammo, FTransform trans);

	UFUNCTION(Reliable, NetMulticast)
	void FireWeaponMulticast(TSubclassOf<AActor> Ammo, FTransform trans);
	void FireWeaponMulticast_Implementation(TSubclassOf<AActor> Ammo, FTransform trans);

	/////// WaistPitch //////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void WraistPitchServer(float pitch);
	void WraistPitchServer_Implementation(float pitch);
	bool WraistPitchServer_Validate(float pitch);

	/////// FireAnimStand //////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void StandFireAnimServer();
	void StandFireAnimServer_Implementation();
	bool StandFireAnimServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void StandFireAnimMulticast();
	void StandFireAnimMulticast_Implementation();


	/////// CrouchFire //////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void CrouchFireServer();
	void CrouchFireServer_Implementation();
	bool CrouchFireServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void CrouchFireMulticast();
	void CrouchFireMulticast_Implementation();

	/////// ProneFire //////////////////////

	UFUNCTION(Reliable, Server, WithValidation)
	void ProneFireServer();
	void ProneFireServer_Implementation();
	bool ProneFireServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void ProneFireMulticast();
	void ProneFireMulticast_Implementation();

private:
	UPROPERTY()
	class UMPPlayerAnimInstance* PlayerAnim;
};
