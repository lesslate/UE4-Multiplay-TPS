// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
#include "GameFramework/Actor.h"
#include "JetSpawner.generated.h"

UCLASS()
class MP_API AJetSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJetSpawner();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=JetClass)
	TSubclassOf<class AJet> JetClass;

	UFUNCTION(Reliable, Server, WithValidation)
	void SpawnJetServer();
	void SpawnJetServer_Implementation();
	bool SpawnJetServer_Validate();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};