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

	virtual void BeginPlay() override;

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


	void GetControlledPawnReference();

	/* How close the AI should get to player before stoping movement */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AIDistanceToPlayer;

public:
	AChuckinAIController();

	// Begin Actor interface
	virtual void Tick(float Delta) override;

	// Override method that handles when movement is completed
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:

	class AChuckinProtoPawn* PlayerPawn;
	class AChuckinAI* ControlledPawn;
};
