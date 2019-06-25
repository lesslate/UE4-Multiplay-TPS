// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class MP_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BulletCollision;

	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* BulletParticles;

	UPROPERTY(EditDefaultsOnly)
	float Velocity;

	UPROPERTY(EditDefaultsOnly)
	float GravityDelay;

	UPROPERTY(EditDefaultsOnly)
	float GravityScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BulletExpiry;

	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ApplyGravity();
};
