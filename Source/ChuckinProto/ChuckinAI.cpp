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
	//MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AChuckinChickin::OnOverlapBegin);
	RootComponent = MeshComp;

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>("MovementComp");
	//MovementComp->rota

	HealthComp = CreateDefaultSubobject<UChuckinHealthComponent>("HealthComp");
}


