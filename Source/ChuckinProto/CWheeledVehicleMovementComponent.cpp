// Fill out your copyright notice in the Description page of Project Settings.


#include "CWheeledVehicleMovementComponent.h"
#include "AIWheeledVehicle.h"
#include "Public/DrawDebugHelpers.h"

static int32 DebugAIMovement = 0;
FAutoConsoleVariableRef CVARDebugAIMovement(
	TEXT("CHUCKIN.DebugAIMovement"),
	DebugAIMovement,
	TEXT("Draw Debug Lines for AI Movement"),
	ECVF_Cheat);

void UCWheeledVehicleMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	//UE_LOG(LogTemp, Warning, TEXT("MOVE: %s"), *MoveVelocity.ToString());
	AActor* OurOwner = GetOwner();
	if (OurOwner)
	{
		FVector ForwardVector = OurOwner->GetActorForwardVector();
		FVector MoveVector = GetNormalNew(MoveVelocity);
		if (DebugAIMovement)
		{
			DrawDebugLine(GetWorld(), OurOwner->GetActorLocation(), OurOwner->GetActorLocation() + MoveVector * 10000.f, FColor::Blue, false, 2.f, 0.f, 4.f);
			//UE_LOG(LogTemp, Warning, TEXT("ForwardVector: %s, MoveVector: %s"), *ForwardVector.ToString(), *MoveVector.ToString());
		}

		// Get Raw Throttle and Steering
		float ThrottleRaw = FVector::DotProduct(ForwardVector, MoveVector);
		float SteeringRaw = FVector::CrossProduct(ForwardVector, MoveVector).Z;
		float Throttle = FMath::Abs(ThrottleRaw);
		float ForwardSpeed = GetForwardSpeed();

		//UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), ForwardSpeed);
		
		if ((ThrottleRaw <= 0) && (ForwardSpeed > 500.f))
		{
			if (!bRawHandbrakeInput)
			{
				SetHandbrakeInput(true);
			}

			SetThrottleInput(0.f);

			if (SteeringRaw >= 0)
			{
				SetSteeringInput(1.f);
			}
			else
			{
				SetSteeringInput(-1.f);
			}
				
		}
		else
		{
			if (bRawHandbrakeInput)
			{
				SetHandbrakeInput(false);
			}
			if (GetForwardSpeed() < 200.f)
			{
				SetThrottleInput(1.f);
				SetSteeringInput(SteeringRaw);
			}
			else
			{
				SetThrottleInput(Throttle);
				SetSteeringInput(SteeringRaw);
			}

		}



		//UE_LOG(LogTemp, Warning, TEXT("Throttle: %f, Steering: %f"), Throttle, Steering);
	}
}

FVector UCWheeledVehicleMovementComponent::GetNormalNew(FVector VectorToNormalize)
{
	float VectorMagnitude = FMath::Sqrt(VectorToNormalize.X * VectorToNormalize.X + VectorToNormalize.Y * VectorToNormalize.Y + VectorToNormalize.Z * VectorToNormalize.Z);

	return FVector(VectorToNormalize.X / VectorMagnitude, VectorToNormalize.Y / VectorMagnitude, VectorToNormalize.Z / VectorMagnitude);

}