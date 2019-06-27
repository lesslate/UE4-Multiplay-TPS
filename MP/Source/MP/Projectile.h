// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class MP_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* ProjectileCollision;

	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* ProjectileParticles;

	UPROPERTY(EditDefaultsOnly)
	float ProjectileSpeed;

	UPROPERTY(EditDefaultsOnly)
	float GravityDelay;

	UPROPERTY(EditDefaultsOnly)
	float GravityScale;


	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ApplyGravity();
};

