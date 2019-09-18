// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChuckinPowerupActor.generated.h"

UCLASS()
class CHUCKINPROTO_API AChuckinPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChuckinPowerupActor();

	virtual void ActivatePowerup(AActor* PowerActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	// Implement in blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	UFUNCTION()
	virtual void OnTickPowerup();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class URotatingMovementComponent* RotatingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UPointLightComponent* LightComp;

	/* Time between powerup ticks */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	/* Total times we apply the powerup effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNrOfTicks;

	// Total number of ticks applied
	int32 TicksSoFar;

	FTimerHandle TimerHandle_PowerupTick;

	bool bIsPowerupActive;

	// Which Actor we are powering up
	class AActor* PowerupTargetActor;

};
