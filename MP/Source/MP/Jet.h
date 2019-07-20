// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MP.h"
#include "GameFramework/Character.h"
#include "Jet.generated.h"

UCLASS()
class MP_API AJet : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	bool CanBombing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* JetStatic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAudioComponent* JetAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* BombSphere;

	UPROPERTY(EditAnywhere, Category=Bomb)
	TSubclassOf<class ABomb> BombClass;
private:
	UFUNCTION(Reliable, Server, WithValidation)
	void BombingServer();
	void BombingServer_Implementation();
	bool BombingServer_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void BombingMulticast();
	void BombingMulticast_Implementation();


	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
