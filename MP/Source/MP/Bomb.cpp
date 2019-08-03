// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(RootComponent);

	BombSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BombSphere"));
	BombSphere->SetupAttachment(BombMesh);

	BombParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BombParticle"));
	BombParticle->SetupAttachment(BombMesh);

	static ConstructorHelpers::FObjectFinder<USoundCue>EXPLO(TEXT("SoundCue'/Game/Sound/Explosioncue.Explosioncue'"));
	if (EXPLO.Succeeded())
	{
		ExplosionCue = EXPLO.Object;
	}
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
	BombSphere->OnComponentBeginOverlap.AddDynamic(this, &ABomb::BombOverlap);
}

void ABomb::BombOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FVector BombLocation = BombSphere->GetComponentLocation();
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionCue, BombLocation);

	BombParticle->Activate();

	UGameplayStatics::ApplyRadialDamage(GetWorld(), 3000.0f, BombLocation, 1000.0f, nullptr, TArray<AActor*>(), this, false);
}
