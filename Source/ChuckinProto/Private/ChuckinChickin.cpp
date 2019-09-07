// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinChickin.h"
#include "Classes/Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChuckinChickin::AChuckinChickin()
{

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	//MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AChuckinChickin::OnOverlapBegin);
	RootComponent = MeshComp;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 2000.f;
	MovementComp->MaxSpeed = 2000.f;
	MovementComp->bShouldBounce = true;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	SecondsTillExplode = 1.f;
}

// Called when the game starts or when spawned
void AChuckinChickin::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Begin Play Chicken!"));
	OnDestroyed.AddDynamic(this, &AChuckinChickin::MyOnDestroyed);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeUntilExplode, this, &AChuckinChickin::Explode, SecondsTillExplode);

	// This will make the chicken ignore the "instigator" actor, which is the car who shot the chicken.
	if (Instigator)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Instigator: %s"), *Instigator->GetName());
		MeshComp->IgnoreActorWhenMoving(Instigator, true);
	}
	
}


void AChuckinChickin::Explode()
{
	UGameplayStatics::ApplyRadialDamage(this, 50.f, GetActorLocation(), 300.f, nullptr, TArray<AActor*>(), this, this->GetInstigatorController(), true, ECC_Visibility);
	RadialForceComp->FireImpulse();

	Destroy();
}

void AChuckinChickin::MyOnDestroyed(AActor* DestroyedActor)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
}




