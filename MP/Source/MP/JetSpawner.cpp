// Fill out your copyright notice in the Description page of Project Settings.


#include "JetSpawner.h"
#include "Jet.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "MPGameMode.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AJetSpawner::AJetSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJetSpawner::BeginPlay()
{
	Super::BeginPlay();

	AMPGameMode* GameMode = Cast<AMPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		GameMode->BombStart.AddUObject(this, &AJetSpawner::SpawnJetServer);
	}
}

void AJetSpawner::SpawnJetServer_Implementation()
{
	GetWorld()->SpawnActor<AJet>(JetClass,GetActorTransform());
}
bool AJetSpawner::SpawnJetServer_Validate()
{
	return true;
}