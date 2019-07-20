// Fill out your copyright notice in the Description page of Project Settings.


#include "Jet.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Bomb.h"

// Sets default values
AJet::AJet()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CanBombing = false;

	JetStatic = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetStaticMesh"));
	JetStatic->SetupAttachment(RootComponent);

	JetAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetAudio"));
	JetAudio->SetupAttachment(JetStatic);

	BombSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BombSphere"));
	BombSphere->SetupAttachment(JetStatic);

	
	
}

// Called when the game starts or when spawned
void AJet::BeginPlay()
{
	Super::BeginPlay();


	FTimerDelegate StopBombing;
	FTimerHandle StopTimer;
	
	FTimerDelegate StartBombing;
	FTimerHandle StartTimer;


	StopBombing.BindLambda([this]()
	{
		CanBombing = false;
	
	});


	StartBombing.BindLambda([this]()
	{
		CanBombing = true;
		BombingServer();
	
	});


	GetWorld()->GetTimerManager().SetTimer(StartTimer, StartBombing, 2.5f, false);
	GetWorld()->GetTimerManager().SetTimer(StopTimer, StopBombing, 5.0f, false);
}


void AJet::BombingServer_Implementation()
{
	FTimerHandle Timer;
	if (CanBombing)
	{
		BombingMulticast();
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AJet::BombingServer , 0.3f, false);
	}
}

bool AJet::BombingServer_Validate()
{
	return true;
	
}

void AJet::BombingMulticast_Implementation()
{
	FTransform BombTransform = BombSphere->GetComponentTransform();
	
	//UKismetMathLibrary::Conv_VectorToTransform();
	
	GetWorld()->SpawnActor<ABomb>(BombClass, BombTransform);
}

// Called every frame
void AJet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FRotator JetRotation = GetActorRotation();
	FVector JetForwardVector = GetActorForwardVector();
	
	GetCharacterMovement()->AddInputVector(JetForwardVector);
}

void AJet::Destroyed()
{
	Super::Destroyed();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}