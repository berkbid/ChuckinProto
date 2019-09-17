// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ChuckinAIController.generated.h"

/**
 * 
 */
UCLASS()
class CHUCKINPROTO_API AChuckinAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	void FireAtPlayer();

	FTimerHandle TimerHandle_TimeBetweenShots;

	FTimerHandle TimerHandle_RefreshPath;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	TSubclassOf<class AChuckinChickin> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	float AITimeBetweenShotsMin;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	float AITimeBetweenShotsMax;

	float AITimeBetweenShots;

	FVector GetNextPathPoint();

	void RefreshPath();

	FVector NextPathPoint;


public:
	AChuckinAIController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	virtual void BeginPlay() override;

	class AChuckinProtoPawn* PlayerPawn;
	class AChuckinAI* ControlledPawn;
};
