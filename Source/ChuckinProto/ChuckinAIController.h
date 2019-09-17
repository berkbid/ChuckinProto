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

	FTimerHandle TimerHandle_TimeBetweenMoveTo;

	FTimerHandle TimerHandle_RefreshPath;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	TSubclassOf<class AChuckinChickin> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	float AITimeBetweenShotsMin;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	float AITimeBetweenShotsMax;

	float AITimeBetweenShots;

	void MoveTowardsPlayer();

	FVector NextPathPoint;

	void GetPlayerReference();


public:
	AChuckinAIController();

	// Begin Actor interface
	virtual void Tick(float Delta) override;

	// Override method that handles when movement is completed
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	virtual void BeginPlay() override;

	class AChuckinProtoPawn* PlayerPawn;
	class AChuckinAI* ControlledPawn;
};
