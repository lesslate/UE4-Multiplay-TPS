// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JetSpawner2.generated.h"

UCLASS()
class MP_API AJetSpawner2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJetSpawner2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JetClass)
	TSubclassOf<class AJet> JetClass;

	UFUNCTION(Reliable, Server, WithValidation)
	void SpawnJetServer();
	void SpawnJetServer_Implementation();
	bool SpawnJetServer_Validate();

};
