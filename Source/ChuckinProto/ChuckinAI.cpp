// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinAI.h"
#include "Classes/Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "ChuckinHealthComponent.h"

// Sets default values
AChuckinAI::AChuckinAI()
{

	//// Craete Azimuth Gimbal to attach Spring Arm to
	//AzimuthGimbal = CreateDefaultSubobject<USceneComponent>("AzimuthGimbal");
	////AzimuthGimbal->SetupAttachment(RootComponent);
	//RootComponent = AzimuthGimbal;


	// Create and setup a StaticMeshComponent
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	//MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AChuckinChickin::OnOverlapBegin);
	RootComponent = MeshComp;
	//MeshComp->SetupAttachment(RootComponent);

	HealthComp = CreateDefaultSubobject<UChuckinHealthComponent>("HealthComp");

}

// Called when the game starts or when spawned
void AChuckinAI::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called to bind functionality to input
void AChuckinAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

