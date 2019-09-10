// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinChickin.h"
#include "Classes/Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Classes/Sound/SoundBase.h"

static int32 DebugChickenDrawing = 0;
FAutoConsoleVariableRef CVARDebugChickenDrawing(
	TEXT("CHUCKIN.DebugChicken"),
	DebugChickenDrawing,
	TEXT("Draw Debug Lines for Chicken"),
	ECVF_Cheat);

// Sets default values
AChuckinChickin::AChuckinChickin()
{
	// Create and setup a StaticMeshComponent
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

	// Explosion default values
	SecondsTillExplode = 1.f;
	DamageRadius = 300.f;
	bIsExploded = false;
	bExpldeOnHit = false;

}

void AChuckinChickin::LaunchProjectile(float speed)
{
	float Time = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Warning, TEXT("%f: Projectile Fires at %f"), Time, speed);
	MovementComp->SetVelocityInLocalSpace(FVector::ForwardVector * speed);
	MovementComp->Activate(true);
}

// Called when the game starts or when spawned
void AChuckinChickin::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Begin Play Chicken!"));
	OnDestroyed.AddDynamic(this, &AChuckinChickin::MyOnDestroyed);

	// Only hook onto the OnComponentHit event if we want to explode on hit
	if (bExpldeOnHit)
	{
		MeshComp->OnComponentHit.AddDynamic(this, &AChuckinChickin::OnCompHit);
	}
	else
	{
		GetWorldTimerManager().SetTimer(TimerHandle_TimeUntilExplode, this, &AChuckinChickin::Explode, SecondsTillExplode);
	}
	
	
	// This will make the chicken ignore the "instigator" actor, which is the car who shot the chicken.
	if (Instigator)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found Instigator: %s"), *Instigator->GetName());
		MeshComp->IgnoreActorWhenMoving(Instigator, true);
	}
	

}


void AChuckinChickin::Explode()
{
	UGameplayStatics::ApplyRadialDamage(this, 50.f, GetActorLocation(), DamageRadius, nullptr, TArray<AActor*>(), this, this->GetInstigatorController(), true, ECC_Visibility);
	RadialForceComp->FireImpulse();

	// Debug sphere to show radius of the Radial Damage from chicken
	if (DebugChickenDrawing)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 2.0f, 0, 3.0f);
	}
	
	// Destroys this actor and fires the MyOnDestroyed even that we hooked into
	Destroy();
}

// This event will only be called if bExplodeOnHit is set to true
void AChuckinChickin::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Chicken OnComponentHit Event Fired!!"));
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSoundEffect, GetActorLocation());
	if (!bIsExploded)
	{
		bIsExploded = true;
		Explode();
	}
}

void AChuckinChickin::MyOnDestroyed(AActor* DestroyedActor)
{
	// Plays explosion effect when destroyed
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

}




