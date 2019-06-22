// Fill out your copyright notice in the Description page of Project Settings.

#include "MPPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "MPPlayerAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Runtime/Engine/Public/TimerManager.h"

// Sets default values
AMPPlayer::AMPPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	IsProne = false;
	IsCrouch = false;

	GetCharacterMovement()->JumpZVelocity = 350.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	bUseControllerRotationYaw = false;

	// 스프링암 생성
	TPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArm"));
	TPSpringArm->SetupAttachment(RootComponent);
	TPSpringArm->TargetArmLength = 300.0f;
	TPSpringArm->bUsePawnControlRotation = true;
	TPSpringArm->bInheritPitch = true;
	TPSpringArm->bInheritRoll = true;
	TPSpringArm->bInheritYaw = true;
	TPSpringArm->bDoCollisionTest = true;


	// 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(TPSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 애니메이션 블루프린트 속성지정
	static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerAnim(TEXT("AnimBlueprint'/Game/Character/PlayerAnimBP.PlayerAnimBP_C'")); // _C를 붙여 클래스정보를 가져옴
	if (PlayerAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(PlayerAnim.Class);
	}

	// 스켈레탈 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMesh(TEXT("SkeletalMesh'/Game/Soldier_01/meshes/CH_soldier_01.CH_soldier_01'"));
	if (PlayerMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerMesh.Object);
	}

	SprintSpeedMultiplier = 1.5f;
}

// Called when the game starts or when spawned
void AMPPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMPPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMPPlayer::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMPPlayer::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMPPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMPPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMPPlayer::StartSprintServer);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMPPlayer::StopSprintServer);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMPPlayer::CrouchServer);
	PlayerInputComponent->BindAction("Prone", IE_Released, this, &AMPPlayer::ProneServer);

}

void AMPPlayer::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMPPlayer::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

////////////////Sprint///////////////////////////////////

void AMPPlayer::StartSprintServer_Implementation()
{
	StartSprintMulticast();
}

bool AMPPlayer::StartSprintServer_Validate()
{
	return true;
}

void AMPPlayer::StartSprintMulticast_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
	/*GetCharacterMovement()->MaxWalkSpeedCrouched *= SprintSpeedMultiplier;*/
	IsSprint = true;
}

void AMPPlayer::StopSprintServer_Implementation()
{
	StopSprintMulticast();
}

bool AMPPlayer::StopSprintServer_Validate()
{
	return true;
}

void AMPPlayer::StopSprintMulticast_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	
	IsSprint = false;
}

//////////// Crouch ///////////////////////
void AMPPlayer::CrouchServer_Implementation()
{
	CrouchMulticast();
}

bool AMPPlayer::CrouchServer_Validate()
{
	return true;
}

void AMPPlayer::CrouchMulticast_Implementation()
{
	if (!IsSprint)
	{
		if (IsProne)
		{
			GetCharacterMovement()->MaxWalkSpeedCrouched = 0;
			IsCrouch = true;
			IsProne = false;
			GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::SetCrouchMovement, 1.3f, false);
		}
		else
		{
			if (!IsCrouch)
			{
				Crouch();
				IsCrouch = true;
			}
			else
			{
				UnCrouch();
				IsCrouch = false;
			}

		}
	}
}

//////////// Prone ///////////////////////


void AMPPlayer::ProneServer_Implementation()
{
	ProneMulticast();
}

bool AMPPlayer::ProneServer_Validate()
{
	return true;
}

void AMPPlayer::ProneMulticast_Implementation()
{
	if (!IsProne && !IsSprint && IsCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 0;
		IsProne = true;
		GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::SetProneMovement, 1.3f, false);
	}
}


void AMPPlayer::SetCrouchMovement()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void AMPPlayer::SetProneMovement()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = 70.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f);
}

//Property Replicate
void AMPPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPPlayer, IsProne);
	DOREPLIFETIME(AMPPlayer, IsCrouch);
	DOREPLIFETIME(AMPPlayer, IsAiming);
	DOREPLIFETIME(AMPPlayer, IsSprint);
}

