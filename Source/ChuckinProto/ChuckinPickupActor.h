// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChuckinPickupActor.generated.h"

UCLASS()
class CHUCKINPROTO_API AChuckinPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChuckinPickupActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent* DecalComp;


	// This is EditInstanceOnly so there is no default powerupactor attached to pickupactors, instead...
	// When you place a pickupactor in the world, you choose the powerupclass for that instance in the world
	//UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	// Use EditDefaultsonly for now since we only have one type of powerup, the health powerup
	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
	TSubclassOf<class AChuckinPowerupActor> PowerUpClass;

	class AChuckinPowerupActor* PowerUpInstance;

	void Respawn();

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

};
