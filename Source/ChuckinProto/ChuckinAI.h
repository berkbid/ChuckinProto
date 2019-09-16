// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChuckinAI.generated.h"

UCLASS()
class CHUCKINPROTO_API AChuckinAI : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChuckinAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* MeshComp;

	/** Azimuth Gimbal to attach Spring arm for camera to, for x direction mouse movement */
	//UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class USceneComponent* AzimuthGimbal;

		/** Health component for player health */
	UPROPERTY(Category = Health, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UChuckinHealthComponent* HealthComp;


public:	

};
