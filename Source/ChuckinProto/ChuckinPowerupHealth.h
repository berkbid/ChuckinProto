// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChuckinPowerupActor.h"
#include "ChuckinPowerupHealth.generated.h"

/**
 * 
 */
UCLASS()
class CHUCKINPROTO_API AChuckinPowerupHealth : public AChuckinPowerupActor
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(EditDefaultsOnly, Category = "HealthActor")
	float HealAmount;

public:

	AChuckinPowerupHealth();

	virtual void OnTickPowerup() override;
};
