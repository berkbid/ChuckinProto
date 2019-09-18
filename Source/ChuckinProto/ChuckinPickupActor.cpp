// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "ChuckinPowerupActor.h"
#include "TimerManager.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinAI.h"

// Sets default values
AChuckinPickupActor::AChuckinPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetSphereRadius(75.f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>("DecalComp");
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(64.f, 75.f, 75.f);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 5.f;

}

// Called when the game starts or when spawned
void AChuckinPickupActor::BeginPlay()
{
	Super::BeginPlay();

	Respawn();
}

void AChuckinPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!OtherActor || !PowerUpInstance) { return; }

	AChuckinProtoPawn* ActorPawn = Cast<AChuckinProtoPawn>(OtherActor);
	AChuckinAI* AIPawn = Cast<AChuckinAI>(OtherActor);

	// Allow healing on both player and AI types of Actors only
	if (ActorPawn || AIPawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Pickup Overlapped With: %s"), *OtherActor->GetName());
		// pass in overlapping actor!
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AChuckinPickupActor::Respawn, CooldownDuration);
	}
}



void AChuckinPickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s, Please update your Blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerUpInstance = GetWorld()->SpawnActor<AChuckinPowerupActor>(PowerUpClass, GetTransform(), SpawnParams);

	FVector TempLocation = GetActorLocation();
	TempLocation.Z += 50.f;
	PowerUpInstance->SetActorRelativeLocation(TempLocation);

}


