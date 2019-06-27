// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpeed = 3000.0f;
	GravityDelay = 0.1f;
	GravityScale = 3.5f;

	ProjectileCollision = CreateDefaultSubobject<UBoxComponent>(FName("BulletCollision"));
	ProjectileCollision->InitBoxExtent(FVector(2.0f));
	ProjectileCollision->BodyInstance.SetCollisionProfileName("Bullet");
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnBulletHit);
	SetRootComponent(ProjectileCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileCollision;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BulletMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);

	ProjectileParticles = CreateDefaultSubobject<UParticleSystemComponent>(FName("ProjectileParticles"));
	ProjectileParticles->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// After GravityDelay Bullet drop
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &AProjectile::ApplyGravity, GravityDelay, false);

}

void AProjectile::OnBulletHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
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

void AProjectile::ApplyGravity()
{
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}

