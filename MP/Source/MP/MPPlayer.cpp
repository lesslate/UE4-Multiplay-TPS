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
#include "Bullet.h"
#include "MPPlayerController.h"
#include "ScopeWidget.h"
#include "WidgetComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "UserWidget.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Projectile.h"
#include "Components/SphereComponent.h"

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

	// 무기 소켓
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("Weapon_Socket"));

	// 스프링암 생성
	TPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArm"));
	TPSpringArm->SetupAttachment(RootComponent);
	TPSpringArm->TargetArmLength = 300.0f;
	TPSpringArm->bUsePawnControlRotation = true;
	TPSpringArm->bInheritPitch = true;
	TPSpringArm->bInheritRoll = true;
	TPSpringArm->bInheritYaw = true;
	TPSpringArm->bDoCollisionTest = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(WeaponMesh);


	// 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(TPSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FPCamera->SetupAttachment(TPSpringArm, USpringArmComponent::SocketName);
	FPCamera->bUsePawnControlRotation = false;

	WeaponCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("WeaponCamera"));
	WeaponCamera->SetupAttachment(Sphere);
	WeaponCamera->bUsePawnControlRotation = true;


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

	static ConstructorHelpers::FClassFinder<UScopeWidget> ScopeWidgetC(TEXT("WidgetBlueprint'/Game/Widget/BP_Scope.BP_Scope_C'"));
	if (ScopeWidgetC.Succeeded())
	{
		ScopeWidgetClass = ScopeWidgetC.Class;
	}

	SprintSpeedMultiplier = 1.5f;
}

// Called when the game starts or when spawned
void AMPPlayer::BeginPlay()
{
	Super::BeginPlay();

	AddScopeWidget();
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

	PlayerInputComponent->BindAxis("Turn", this, &AMPPlayer::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMPPlayer::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMPPlayer::StartSprintServer);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMPPlayer::StopSprintServer);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMPPlayer::CrouchServer);
	PlayerInputComponent->BindAction("Prone", IE_Released, this, &AMPPlayer::ProneServer);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AMPPlayer::Aiming);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMPPlayer::Fire);

}
void AMPPlayer::AddControllerPitchInput(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddPitchInput(Val);
	}
	WraistPitch = GetControlRotation().Pitch;
	if(FMath::Abs(WraistPitch - PreviousWraistPitch)>1)
	{
		WraistPitchServer(WraistPitch);
		PreviousWraistPitch = WraistPitch;
	}
	
}

void AMPPlayer::AddControllerYawInput(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddYawInput(Val);
	}
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
	if (!IsAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
		/*GetCharacterMovement()->MaxWalkSpeedCrouched *= SprintSpeedMultiplier;*/
		IsSprint = true;
	}
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

////////Aiming ///////////////////////

void AMPPlayer::Aiming()
{
	if (!IsAiming)
	{
		AimingServer(true); 
		IsAiming = true;
		
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f);
		//FollowCamera->SetFieldOfView(10.0f);
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		ScopeWidget->SetVisibility(ESlateVisibility::Visible);
		FollowCamera->Deactivate();
		WeaponCamera->Activate();
	}
	else
	{
		AimingServer(false);
		IsAiming = false;
	
		//FollowCamera->SetFieldOfView(90.0f);
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		ScopeWidget->SetVisibility(ESlateVisibility::Hidden);
		WeaponCamera->Deactivate();
		FollowCamera->Activate();
	
		if (IsProne)
		{
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f);
		}
		else
		{
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		}
	}
}

void AMPPlayer::AimingServer_Implementation(bool Aiming)
{
	IsAiming = Aiming;
	if (IsAiming)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

bool AMPPlayer::AimingServer_Validate(bool Aiming)
{
	return true;
}

//////// Fire /////////////////////////////

void AMPPlayer::Fire()
{
	if (IsAiming)
	{

		//if (BulletClass != nullptr)
		//{
		//	const FRotator SpawnRotation = GetControlRotation();
		//	LOG(Warning, TEXT("Fire"));
		//	const FVector SpawnLocation = WeaponCamera->GetForwardVector()*100;

		//	UWorld* const World = GetWorld();
		//	if (World != nullptr)
		//	{
		//		LOG(Warning, TEXT("Spawn"));
		//		AActor* Bullet = World->SpawnActor<AActor>(BulletClass, SpawnLocation, SpawnRotation);

		//	}
		//}
	}
}

void AMPPlayer::FireServer_Implementation()
{
	FireMulticast();
}

bool AMPPlayer::FireServer_Validate()
{
	return true;
}

void AMPPlayer::FireMulticast_Implementation()
{
	
}
//////// Wraist Pitch
void AMPPlayer::WraistPitchServer_Implementation(float pitch)
{
	WraistPitch = pitch;
}

bool AMPPlayer::WraistPitchServer_Validate(float pitch)
{
	return true;
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

void AMPPlayer::AddScopeWidget()
{
	ScopeWidget = CreateWidget<UScopeWidget>(GetWorld(), ScopeWidgetClass);
	ScopeWidget->AddToViewport();
	ScopeWidget->SetVisibility(ESlateVisibility::Hidden);
}



//Property Replicate
void AMPPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPPlayer, IsProne);
	DOREPLIFETIME(AMPPlayer, IsCrouch);
	DOREPLIFETIME(AMPPlayer, IsAiming);
	DOREPLIFETIME(AMPPlayer, IsSprint);
	DOREPLIFETIME(AMPPlayer, WraistPitch);
	
}


