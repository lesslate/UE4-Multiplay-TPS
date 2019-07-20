// Fill out your copyright notice in the Description page of Project Settings.


#include "JetSpawner2.h"
#include "Jet.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "MPGameMode.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AJetSpawner2::AJetSpawner2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJetSpawner2::BeginPlay()
{
	Super::BeginPlay();

	AMPGameMode* GameMode = Cast<AMPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		GameMode->BombStart2.AddUObject(this, &AJetSpawner2::SpawnJetServer);
	}

}

void AJetSpawner2::SpawnJetServer_Implementation()
{
	GetWorld()->SpawnActor<AJet>(JetClass, GetActorTransform());
}
bool AJetSpawner2::SpawnJetServer_Validate()
{
	return true;
}