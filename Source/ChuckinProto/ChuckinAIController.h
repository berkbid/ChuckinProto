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

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	TSubclassOf<class AChuckinChickin> ProjectileClass;

public:


private:
	virtual void BeginPlay() override;

	class AChuckinProtoPawn* PlayerPawn;
	class AChuckinAI* ControlledPawn;
};
