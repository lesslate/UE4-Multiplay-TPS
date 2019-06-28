// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	Sphere->SetupAttachment(RootComponent);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);

	BulletParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticles"));
	BulletParticles->SetupAttachment(RootComponent);

	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	InitialLifeSpan = 5.0f;

	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace + (Velocity*DeltaTime);

	FVector Gravity = UKismetMathLibrary::MakeVector(0, 0, -980.0);
	FVector CurrentGravity = Gravity * DeltaTime;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel2, CollisionParams))
	{
		AActor* HitActor=HitResult.GetActor();
		if (HitResult.GetActor())
		{
			DrawDebugSolidBox(GetWorld(), HitResult.ImpactPoint, FVector(10.0f), FColor::Blue, true);
			
			if (HitResult.BoneName != NAME_None)
			{
				LOG(Warning, TEXT("%s"), *HitResult.BoneName.ToString());
			}
			UGameplayStatics::ApplyPointDamage(HitActor, 50.0f, HitActor->GetActorLocation(), HitResult, nullptr, this, nullptr); // 포인트 데미지
			Destroy();
		}
	}
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 255), false, 10.0f);

	SetActorLocation(EndTrace);
	Velocity = Velocity + CurrentGravity;

}
