// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "CWheeledVehicleMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CHUCKINPROTO_API UCWheeledVehicleMovementComponent : public UWheeledVehicleMovementComponent4W
{
	GENERATED_BODY()
	

public:
	/** path following: request new velocity */
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

private:

	FVector GetNormalNew(FVector VectorToNormalize);
};
