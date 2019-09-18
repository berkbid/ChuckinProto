// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPowerupActor.h"
#include "Public/TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/PointLightComponent.h"


// Sets default values
AChuckinPowerupActor::AChuckinPowerupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	LightComp = CreateDefaultSubobject<UPointLightComponent>("LightComp");
	LightComp->CastShadows = false;
	LightComp->SetupAttachment(RootComponent);


	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>("RotatingComp");


	PowerupInterval = 1.f;
	TotalNrOfTicks = 2.f;
	PowerupTargetActor = nullptr;

	bIsPowerupActive = false;

}

void AChuckinPowerupActor::ActivatePowerup(AActor* PowerActor)
{
	if (!PowerActor) { return; }
	//UE_LOG(LogTemp, Warning, TEXT("ACTIVATING POWERUP FOR %s"), *PowerActor->GetName());

	PowerupTargetActor = PowerActor;
	bIsPowerupActive = true;
	MeshComp->SetVisibility(false, true);
	

	if (PowerupInterval > 0.f)
	{

		// Ticks for first time after interval 
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AChuckinPowerupActor::OnTickPowerup, PowerupInterval, true, 0.f);
	}
	else
	{
		OnTickPowerup();
	}
}

void AChuckinPowerupActor::OnTickPowerup()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTickPowerupCalled from parent"));
	TicksSoFar++;

	if (TicksSoFar >= TotalNrOfTicks)
	{
		Destroy();
	}
}

