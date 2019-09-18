// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPowerupHealth.h"
#include "Kismet/GameplayStatics.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinHealthComponent.h"

AChuckinPowerupHealth::AChuckinPowerupHealth()
{
	HealAmount = 50.f;

	PowerupInterval = 0.f;
	TotalNrOfTicks = 1.f;
}

void AChuckinPowerupHealth::OnTickPowerup()
{
	TicksSoFar++;

	if (PowerupTargetActor)
	{
		UChuckinHealthComponent* HealthComp = Cast<UChuckinHealthComponent>(PowerupTargetActor->GetComponentByClass(UChuckinHealthComponent::StaticClass()));

		if (HealthComp)
		{
			HealthComp->Heal(HealAmount);
		}
	}

	if (TicksSoFar >= TotalNrOfTicks)
	{
		Destroy();
	}
}
