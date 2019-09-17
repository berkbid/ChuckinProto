// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinChickin.h"
#include "Classes/Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Classes/Sound/SoundBase.h"
#include "Components/SceneComponent.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinAI.h"
#include "Engine/StaticMeshActor.h"

static int32 DebugChickenDrawing = 0;
FAutoConsoleVariableRef CVARDebugChickenDrawing(
	TEXT("CHUCKIN.DebugChicken"),
	DebugChickenDrawing,
	TEXT("Draw Debug Lines for Chicken"),
	ECVF_Cheat);

// Sets default values
AChuckinChickin::AChuckinChickin()
{

	// Craete Azimuth Gimbal to attach Spring Arm to
	//AzimuthGimbal = CreateDefaultSubobject<USceneComponent>("AzimuthGimbal");
	//RootComponent = AzimuthGimbal;

	// Create and setup a StaticMeshComponent
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	//MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AChuckinChickin::OnOverlapBegin);
	RootComponent = MeshComp;
	//MeshComp->SetupAttachment(RootComponent);
	

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	//MovementComp->InitialSpeed = 2000.f;
	//MovementComp->MaxSpeed = 2000.f;
	MovementComp->bShouldBounce = true;
	MovementComp->bAutoActivate = false;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	// Explosion default values
	SecondsTillExplode = 1.f;
	DamageRadius = 300.f;
	DamageAmount = 50.f;
	bIsExploded = false;
	bExpldeOnHit = false;
	bHasHit = false;

}

void AChuckinChickin::LaunchProjectile(float speed)
{
	//float Time = GetWorld()->GetTimeSeconds();
	//UE_LOG(LogTemp, Warning, TEXT("%f: Projectile Fires at %f"), Time, speed);
	MovementComp->SetVelocityInLocalSpace(FVector::ForwardVector * speed);
	MovementComp->Activate(true);
}

// Called when the game starts or when spawned
void AChuckinChickin::BeginPlay()
{
	Super::BeginPlay();

	OnDestroyed.AddDynamic(this, &AChuckinChickin::MyOnDestroyed);
	MeshComp->OnComponentHit.AddDynamic(this, &AChuckinChickin::OnCompHit);
	// Only hook onto the OnComponentHit event if we want to explode on hit
	if (!bExpldeOnHit)
	{		
		GetWorldTimerManager().SetTimer(TimerHandle_TimeUntilExplode, this, &AChuckinChickin::Explode, SecondsTillExplode);
	}	
	// This will make the chicken ignore the "instigator" actor, which is the car who shot the chicken.
	if (Instigator)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found Instigator: %s"), *Instigator->GetName());

		if (Cast<AChuckinAI>(Instigator))
		{
			MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Ignore);
		}
		else
		{
			MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Ignore);
		}
	}

	
}


void AChuckinChickin::Explode()
{
	bIsExploded = true;
	// Can add actors to ignore for applying radial damage
	// Setting GetOwner() here makes the damage IGNORE the owning actor
	UGameplayStatics::ApplyRadialDamage(this, DamageAmount, GetActorLocation(), DamageRadius, nullptr, TArray<AActor*>(), GetOwner(), this->GetInstigatorController(), true, ECC_Visibility);
	
	if (RadialForceComp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Radial force comp found"));
		RadialForceComp->FireImpulse();
	}
	

	 //Debug sphere to show radius of the Radial Damage from chicken
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
	if (!bHasHit)
	{
		bHasHit = true;
	}	

	if (!Cast<AStaticMeshActor>(Hit.Actor))
	{
		if (Cast<AChuckinProtoPawn>(Hit.Actor))
		{
			if (Hit.Actor != GetOwner())
			{
				UE_LOG(LogTemp, Warning, TEXT("Chicken Hit ACTOR: %s"), *Hit.Actor->GetName());
				Explode();
			}
		}
		else if (Cast<AChuckinAI>(Hit.Actor))
		{
			if (Hit.Actor != Instigator)
			{
				UE_LOG(LogTemp, Warning, TEXT("Chicken Hit AI ACTOR: %s, Instigator: %s"), *Hit.Actor->GetName(), *Instigator->GetName());
				Explode();
			}

		}
		else if (Cast<AActor>(Hit.Actor))
		{
			Explode();
		}
	}


	if (bExpldeOnHit)
	{		
		if (!bIsExploded)
		{
			Explode();
		}
	}

}

void AChuckinChickin::MyOnDestroyed(AActor* DestroyedActor)
{
	// Plays explosion effect when destroyed
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSoundEffect, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

}




