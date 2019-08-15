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
#include "MPPlayerAnimInstance.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Widget_Death.h"
#include "Widget_Winner.h"
#include "MPGameMode.h"
#include "Components/WidgetComponent.h"



// Sets default values
AMPPlayer::AMPPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	IsProne = false;
	IsCrouch = false;
	IsReloading = false;

	PlayerMaxHP = 100;
	PlayerHP = PlayerMaxHP;

	CurrentAmmo = 5;
	RemainAmmo = 50;
	
	GetMesh()->VisibilityBasedAnimTickOption= EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones; // Server Refresh Bones ( 서버와 클라이언트 본 Refresh)

	Magazine = 5;
	PlayerMouseSensitivity = 80.0f;


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

	// 총구 스피어
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(WeaponMesh);

	// 무기 콜리전
	WeaponCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WeaponCapsule"));
	WeaponCapsule->SetupAttachment(WeaponMesh);

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

	SprintSpeedMultiplier = 1.5f;
	
	// 오디오 컴포넌트
	PlayerAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerAudio"));
	PlayerAudio->SetupAttachment(GetMesh());

	// 위젯 컴포넌트
	PlayerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerWidget"));
	PlayerWidget->SetupAttachment(RootComponent);

	// 파티클 초기화
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Fire(TEXT("ParticleSystem'/Game/WeaponEffects/AssaultRifle_MF.AssaultRifle_MF'"));
	if (Fire.Succeeded())
	{
		FireParticle = Fire.Object;
	}


	// 사운드 큐
	static ConstructorHelpers::FObjectFinder<USoundCue>FIRECUE(TEXT("SoundCue'/Game/Sound/SniperShot_Cue.SniperShot_Cue'"));
	if (FIRECUE.Succeeded())
	{
		ShotCue = FIRECUE.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>METALCLICK(TEXT("SoundCue'/Game/WeaponEffects/Metal-click_Cue.Metal-click_Cue'"));
	if (METALCLICK.Succeeded())
	{
		MetalClickCue = METALCLICK.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundCue>AIMCUE(TEXT("SoundCue'/Game/Sound/Zoom_Cue.Zoom_Cue'"));
	if (AIMCUE.Succeeded())
	{
		AimCue = AIMCUE.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>HITCUE(TEXT("SoundCue'/Game/Sound/Hit_Cue.Hit_Cue'"));
	if (HITCUE.Succeeded())
	{
		HitCue = HITCUE.Object;
	}


	// 애니메이션 블루프린트 속성지정
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM(TEXT("AnimBlueprint'/Game/Character/PlayerAnimBP.PlayerAnimBP_C'")); // _C를 붙여 클래스정보를 가져옴
	if (ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ANIM.Class);
	}

	// 스켈레탈 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMesh(TEXT("SkeletalMesh'/Game/Soldier_01/meshes/CH_soldier_01.CH_soldier_01'"));
	if (PlayerMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerMesh.Object);
	}


}

void AMPPlayer::ResetDelay()
{
	FireDelay = false;
}



void AMPPlayer::ReloadEnd()
{
	if ((Magazine - CurrentAmmo) > RemainAmmo)
	{
		CurrentAmmo = (RemainAmmo + CurrentAmmo);
		RemainAmmo = 0;
	}
	else
	{
		RemainAmmo = RemainAmmo - (Magazine - CurrentAmmo);
		CurrentAmmo = (Magazine - CurrentAmmo) + CurrentAmmo;
	}
	IsReloading = false;
}


// Called when the game starts or when spawned
void AMPPlayer::BeginPlay()
{
	Super::BeginPlay();

	AddWidget(); // Add Scope Widget

	
}

void AMPPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerAnim = Cast<UMPPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(PlayerAnim != nullptr)

	GameMode = Cast<AMPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	CHECK(GameMode != nullptr)
	if (GameMode != nullptr)
	{
		GameMode->EndGame.AddUObject(this, &AMPPlayer::AddEndGameWidget);
	}
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

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMPPlayer::PlayerCrouch);
	PlayerInputComponent->BindAction("Prone", IE_Released, this, &AMPPlayer::PlayerProne);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AMPPlayer::Aiming);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMPPlayer::Fire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMPPlayer::Reload);

}
void AMPPlayer::AddControllerPitchInput(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		Val = PlayerMouseSensitivity * GetInputAxisValue("LookUp")*UGameplayStatics::GetWorldDeltaSeconds(GetWorld()); // 마우스 감도에 따른 시점변환
		PC->AddPitchInput(Val);
	}
	WraistPitch = GetControlRotation().Pitch;
	if(FMath::Abs(WraistPitch - PreviousWraistPitch)>1) // Pitch값 변화시 서버로 전송 (캐릭터의 상체 Bone 조절)
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
		Val = PlayerMouseSensitivity *GetInputAxisValue("Turn")*UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
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

void AMPPlayer::PlayerCrouch()
{
	
	if (!IsSprint)
	{
		if (IsProne)
		{
			GetCharacterMovement()->MaxWalkSpeedCrouched = 0;
			//IsCrouch = true;
			//IsProne = false;
			if (IsAiming)
			{
				APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
				CameraManager->ViewPitchMin = -30;
				CameraManager->ViewPitchMax = 30; // 카메라 시야 최대 최소 시야각도 조절
			}
			CrouchServer(true,true);
			GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::SetCrouchMovement, 1.3f, false);
			
		}
		else
		{
			if (!IsCrouch)
			{
				Crouch();
				//IsCrouch = true;
				if (IsAiming)
				{
					APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
					CameraManager->ViewPitchMin = -30;
					CameraManager->ViewPitchMax = 30;
				}
				CrouchServer(false,true);
			}
			else
			{
				if (IsAiming)
				{
					APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
					CameraManager->ViewPitchMin = -89.9;
					CameraManager->ViewPitchMax = 89.9;
				}
				UnCrouch();
				
				CrouchServer(false,false);
			}

		}
		
	}
	LOG(Warning, TEXT("%s, %s"), IsCrouch ? TEXT("true") : TEXT("false"),NETMODE_WORLD);
}

void AMPPlayer::CrouchServer_Implementation(bool Prone, bool Crouching)
{
	if (Prone&&Crouching) // Prone 상태 해제
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 0;
		IsProne = false;
		GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::SetCrouchMovement, 1.3f, false);
	}
	else if (Crouching) 
	{
		IsCrouch = true;
	}
	else
	{
		IsCrouch = false;
	}
	
}

bool AMPPlayer::CrouchServer_Validate(bool Prone, bool Crouching)
{
	return true;
}

//////////// Prone ///////////////////////

void AMPPlayer::PlayerProne()
{
	if (!IsProne && !IsSprint && IsCrouch)
	{
		
		GetCharacterMovement()->MaxWalkSpeedCrouched = 0;


		if (IsAiming) // Prone 상태 에임시 카메라 최대최소 각도 
		{
			APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)); 
			CameraManager->ViewPitchMin = -5;
			CameraManager->ViewPitchMax = 30;
		}
		
		ProneServer();
		GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::SetProneMovement, 1.3f, false);
	}
	
}
void AMPPlayer::ProneServer_Implementation()
{
	if (!IsProne && !IsSprint && IsCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 0;
		IsProne = true;

		GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::SetProneMovement, 1.3f, false);
	}
}

bool AMPPlayer::ProneServer_Validate()
{
	return true;
}


////////Aiming ///////////////////////

void AMPPlayer::Aiming()
{
	
	if (!IsDeath && !IsAiming && !IsReloading)
	{
		AimingServer(true);
		IsAiming = true;
		WeaponCapsule->SetGenerateOverlapEvents(true); // Capsule 오버랩시 에임상태 풀리도록함 (Glitch 방지)

		//CameraAngle Limit
		if (IsCrouch)
		{
			APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
			CameraManager->ViewPitchMin = -30;
			CameraManager->ViewPitchMax = 30;
		}
		if (IsProne)
		{
			APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
			CameraManager->ViewPitchMin = -5;
			CameraManager->ViewPitchMax = 30;
		}
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f);
		GetCharacterMovement()->bUseControllerDesiredRotation = true; // 에임 방향으로 캐릭터 회전
		GetCharacterMovement()->bOrientRotationToMovement = false;
		ScopeWidget->SetVisibility(ESlateVisibility::Visible);
		FollowCamera->Deactivate();
		WeaponCamera->Activate(); // 총구 카메라 Activate

		PlayerAudio->SetSound(AimCue);
		PlayerAudio->Play();
	}
	else // 에임 해제
	{
		AimingServer(false);
		IsAiming = false;
		WeaponCapsule->SetGenerateOverlapEvents(false);

		//CameraAngle Reset
		APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
		CameraManager->ViewPitchMin = -89.9f;
		CameraManager->ViewPitchMax = 89.9f;

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
	bool IsMove = GetVelocity().Size()>0; // 움직이지 않을때 발사가능
	
	if (!FireDelay && IsAiming && !IsMove && !IsDeath&& CurrentAmmo!=0)
	{
		APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)); // FireCameraShake
		CameraManager->PlayCameraShake(CameraShakeClass, 1.0f);

		FireDelay = true;
		CurrentAmmo--;
		UGameplayStatics::SpawnEmitterAttached(FireParticle, WeaponMesh, FName("Muzzle"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f)); 

		
		OnFire(); // 발사체 생성

		// Play Animation
		if (IsCrouch)
		{
			if (IsProne)
			{
				PlayerAnim->PlayProneFire();//ProneFire
				ProneFireServer();
			}
			else
			{
				PlayerAnim->PlayCrouchFire();//CrouchFire
				CrouchFireServer();
			}
		}
		else
		{
			PlayerAnim->PlayFireMontage(); //StandFire
			StandFireAnimServer();
		}
		GetWorld()->GetTimerManager().SetTimer(timer, this, &AMPPlayer::ResetDelay, 1.0f, false); // 발사 딜레이 초기화
	}
	if(CurrentAmmo==0)
	{
		PlayerAudio->SetSound(MetalClickCue);
		PlayerAudio->Play();
	}
	
}

void AMPPlayer::OnFire()
{
	FVector SphereLocation= Sphere->GetComponentLocation();
	FRotator CameraRotation = WeaponCamera->GetComponentRotation();
	FVector CameraRotated = CameraRotation.RotateVector(FVector(0, 0, 0));
	FVector StartLocation = (SphereLocation + CameraRotated);

	FTransform Transform=UKismetMathLibrary::MakeTransform(StartLocation, CameraRotation,FVector(1,1,1));

	
	FireSoundServer(StartLocation);

	FVector NewVelocity = WeaponCamera->GetForwardVector()*24000.0f; // 발사체 속도

	FireWeapon(Transform,NewVelocity);
}

void AMPPlayer::FireWeapon(FTransform trans,FVector velocity)
{

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		
		ABullet* Bullet = World->SpawnActor<ABullet>(AmmoClass, trans);
		Bullet->Velocity = FVector(velocity);
		if (HasAuthority())
		{
			FireWeaponMulticast(AmmoClass, trans,velocity);
		}
		else
		{
			FireWeaponServer(AmmoClass, trans, velocity);
		}
	}
}

void AMPPlayer::FireWeaponServer_Implementation(TSubclassOf<AActor> Ammo, FTransform trans,FVector velocity)
{
	FireWeaponMulticast(Ammo, trans,velocity);
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		ABullet* Bullet = World->SpawnActor<ABullet>(AmmoClass, trans);
		Bullet->Velocity = FVector(velocity);
	}
}

bool AMPPlayer::FireWeaponServer_Validate(TSubclassOf<AActor> Ammo, FTransform trans, FVector velocity)
{
	return true;
}

void AMPPlayer::FireWeaponMulticast_Implementation(TSubclassOf<AActor> Ammo, FTransform trans, FVector velocity)
{
	UGameplayStatics::SpawnEmitterAttached(FireParticle, WeaponMesh, FName("Muzzle"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f));
	if (!IsValid(GetController()))
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			ABullet* Bullet = World->SpawnActor<ABullet>(AmmoClass, trans);
			Bullet->Velocity = FVector(velocity);
		}
	}
	
}



//////// Wraist Pitch //////////////////////////////////

void AMPPlayer::WraistPitchServer_Implementation(float pitch)
{
	WraistPitch = pitch;
}

bool AMPPlayer::WraistPitchServer_Validate(float pitch)
{
	return true;
}

/////////// StandFireAnim ///////////////////////

void AMPPlayer::StandFireAnimServer_Implementation()
{
	StandFireAnimMulticast();
}

bool AMPPlayer::StandFireAnimServer_Validate()
{
	return true;
}

void AMPPlayer::StandFireAnimMulticast_Implementation()
{
	PlayerAnim->PlayFireMontage();
}

/////////// CrouchFire///////////////////////////

void AMPPlayer::CrouchFireServer_Implementation()
{
	CrouchFireMulticast();
}

bool AMPPlayer::CrouchFireServer_Validate()
{
	return true;
}

void AMPPlayer::CrouchFireMulticast_Implementation()
{
	PlayerAnim->PlayCrouchFire();
}

///////Prone Fire/////////////////////////////

void AMPPlayer::ProneFireServer_Implementation()
{
	ProneFireMulticast();
}

bool AMPPlayer::ProneFireServer_Validate()
{
	return true;
}

void AMPPlayer::ProneFireMulticast_Implementation()
{
	PlayerAnim->PlayProneFire();
}

///////// Death //////////////////////////////////

void AMPPlayer::Death()
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeWidget->RemoveFromParent();
	WeaponCamera->Deactivate();
	FollowCamera->Activate();
	IsDeath = true;
	

	if (IsCrouch)
	{
		if (IsProne)
		{
			PlayerAnim->PlayProneDeathMontage();
		}
		else 
		{
			PlayerAnim->PlayCrouchDeathMontage();
		}
	}
	else 
	{
		PlayerAnim->PlayDeathMontage();
	}
	DeathMulticast();
	
}

void AMPPlayer::DeathMulticast_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeWidget->RemoveFromParent();
	WeaponCamera->Deactivate();
	FollowCamera->Activate();
	IsDeath = true;
	if (IsCrouch)
	{
		if (IsProne)
		{
			PlayerAnim->PlayProneDeathMontage();
		}
		else
		{
			PlayerAnim->PlayCrouchDeathMontage();
		}
	}
	else
	{
		PlayerAnim->PlayDeathMontage();
	}
	
}

///////////// Reload /////////////////////////

void AMPPlayer::Reload()
{
	if (!IsAiming && !IsReloading && CurrentAmmo != Magazine && RemainAmmo != 0)
	{
		IsReloading = true;
		if (IsCrouch)
		{
			if (IsProne)
			{
				PlayerAnim->PlayProneReloadMontage();
			}
			else
			{
				PlayerAnim->PlayCrouchReloadMontage();
			}
		}
		else
		{
			PlayerAnim->PlayReloadMontage();
		}
		ReloadServer();
	}
}

void AMPPlayer::ReloadServer_Implementation()
{
	
	ReloadMulticast();
}

bool AMPPlayer::ReloadServer_Validate()
{
	return true;
}

void AMPPlayer::ReloadMulticast_Implementation()
{
	IsReloading = true;

	if (IsCrouch)
	{
		if (IsProne)
		{
			
			PlayerAnim->PlayProneReloadMontage();
		}
		else
		{
		
			PlayerAnim->PlayCrouchReloadMontage();
		}
	}
	else
	{
	
		PlayerAnim->PlayReloadMontage();
	}

}
//// Fire Sound///////////////

bool AMPPlayer::FireSoundServer_Validate(FVector SoundLocation)
{
	return true;
}

void AMPPlayer::FireSoundServer_Implementation(FVector SoundLocation)
{
	FireSoundMulticast(SoundLocation);
}

void AMPPlayer::FireSoundMulticast_Implementation(FVector SoundLocation)
{
	UGameplayStatics::PlaySoundAtLocation(this,  ShotCue, SoundLocation);
}

void AMPPlayer::HitSoundClient_Implementation(FVector SoundLocation)
{
	UGameplayStatics::PlaySoundAtLocation(this, HitCue, SoundLocation);
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

void AMPPlayer::AddEndGameWidget_Implementation()
{
	AMPPlayerController* PC = Cast<AMPPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeUIOnly());
	LOG(Warning, TEXT("End %s"), NETMODE_WORLD);
	if (IsDeath)
	{
		DeathWidget = CreateWidget<UWidget_Death>(GetWorld(), DeathWidgetClass);
		DeathWidget->AddToViewport();
		
		
	}
	else
	{
		WinnerWidget = CreateWidget<UWidget_Winner>(GetWorld(), WinnerWidgetClass);
		WinnerWidget->AddToViewport();
	}
	

}

void AMPPlayer::LeftBombing_Implementation()
{
	CHECK(GameMode!=nullptr)
	GameMode->BombStart.Broadcast();
}

bool AMPPlayer::LeftBombing_Validate()
{
	return true;
}

void AMPPlayer::RightBombing_Implementation()
{
	CHECK(GameMode != nullptr)
	GameMode->BombStart2.Broadcast();
}

bool AMPPlayer::RightBombing_Validate()
{
	return true;
}

void AMPPlayer::AddWidget()
{
	ScopeWidget = CreateWidget<UScopeWidget>(GetWorld(), ScopeWidgetClass);
	ScopeWidget->AddToViewport();
	ScopeWidget->SetVisibility(ESlateVisibility::Hidden);

}

float AMPPlayer::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		PlayerHP -= ActualDamage;

		HitSoundClient(GetActorLocation());

		if (IsValid(GetController())&&PlayerHP <= 0.f)
		{
			IsDeath = true;
			ScopeWidget->SetVisibility(ESlateVisibility::Hidden);
			WeaponCamera->Deactivate();
			FollowCamera->Activate();
			Death();
			
			GameMode->EndGame.Broadcast();
			
			//AMPPlayerController* PC = Cast<AMPPlayerController>(GetController());
			//CHECK(PC != nullptr)
			
		
		}
	}
	return ActualDamage;
}

//Property Replicate
void AMPPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPPlayer, IsProne);
	DOREPLIFETIME(AMPPlayer, IsCrouch);
	DOREPLIFETIME(AMPPlayer, IsAiming);
	DOREPLIFETIME(AMPPlayer, IsSprint);
	DOREPLIFETIME(AMPPlayer, IsDeath);
	DOREPLIFETIME(AMPPlayer, IsReloading);
	DOREPLIFETIME(AMPPlayer, WraistPitch);
	DOREPLIFETIME(AMPPlayer, PlayerHP);
	DOREPLIFETIME(AMPPlayer, PlayerMaxHP);
	
}


