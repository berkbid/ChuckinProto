// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChuckinChickin.generated.h"


UCLASS()
class CHUCKINPROTO_API AChuckinChickin : public AActor
{
	GENERATED_BODY()



public:	
	// Sets default values for this actor's properties
	AChuckinChickin();


protected:

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class URadialForceComponent* RadialForceComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void MyOnDestroyed(AActor* DestroyedActor);

	void Explode();

	FTimerHandle TimerHandle_TimeUntilExplode;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float SecondsTillExplode;

public:	

};
