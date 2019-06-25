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

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Velocity = 40000.0f;
	GravityDelay = 0.1f;
	GravityScale = 3.5f;

	BulletCollision = CreateDefaultSubobject<UBoxComponent>(FName("BulletCollision"));
	BulletCollision->InitBoxExtent(FVector(2.0f));
	BulletCollision->BodyInstance.SetCollisionProfileName("Bullet");
	BulletCollision->OnComponentHit.AddDynamic(this, &ABullet::OnBulletHit);
	SetRootComponent(BulletCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BulletCollision;
	ProjectileMovement->InitialSpeed = Velocity;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);

	BulletParticles = CreateDefaultSubobject<UParticleSystemComponent>(FName("ProjectileParticles"));
	BulletParticles->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	// After GravityDelay Bullet drop
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &ABullet::ApplyGravity, GravityDelay, false);

}

void ABullet::OnBulletHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor)
	{
		if (Hit.BoneName != NAME_None)
		{
			LOG(Warning, TEXT("Hit Bone : %s"), *Hit.BoneName.ToString());
		}
		OtherActor->Destroy();

	}
	Destroy();
}

void ABullet::ApplyGravity()
{
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}

