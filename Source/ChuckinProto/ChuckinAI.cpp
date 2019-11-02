// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinAI.h"
#include "Classes/Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "ChuckinHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
AChuckinAI::AChuckinAI()
{


	// Create and setup a StaticMeshComponent
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComp->SetCanEverAffectNavigation(false);
	// Need this so the car doesnt spin out of control
	MeshComp->SetAngularDamping(50.f);
	//MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AChuckinChickin::OnOverlapBegin);
	RootComponent = MeshComp;

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>("MovementComp");
	MovementComp->MaxSpeed = 800.f;
	MovementComp->TurningBoost = 24.f;
	//MovementComp->UpdatedComponent
	MovementComp->SetUpdatedComponent(RootComponent);
	

	HealthComp = CreateDefaultSubobject<UChuckinHealthComponent>("HealthComp");

	
	//bUseControllerRotationPitch = true;
	//bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;
}


