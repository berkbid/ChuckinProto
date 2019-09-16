// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinHealthComponent.h"
#include "GameFramework/Actor.h"
#include "ChuckinProtoGameMode.h"
#include "Engine/World.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinAI.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"

// Sets default values for this component's properties
UChuckinHealthComponent::UChuckinHealthComponent()
{
	DefaultHealth = 100.f;
	MyOwner = nullptr;
	Health = DefaultHealth;

	TeamNum = 255;

}


// Called when the game starts
void UChuckinHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UChuckinHealthComponent::HandleTakeAnyDamage);
	}
	
}


void UChuckinHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s, Took Damage: %f"), *DamagedActor->GetName(), Damage);
	if (Damage <= 0.f || bIsDead)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("%s New Health: %f"), *DamagedActor->GetName(), Health);
	// Check if we are dead with new health amount
	if (Health <= 0.f)
	{
		bIsDead = true;
		if (MyOwner)
		{
			AChuckinProtoGameMode* GM = Cast<AChuckinProtoGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				// Need to broadcast BEFORE destroy() on owner, or we wont have reference
				GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
				
				// Destroy owner
				MyOwner->Destroy();

				// Tell Game Mode to Prepare to respawn
				GM->PrepareForSpawn();
			}
		}

	} 


}

float UChuckinHealthComponent::GetHealth() const
{
	return Health;
}


