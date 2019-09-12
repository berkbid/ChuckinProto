// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinAIController.h"
#include "ChuckinAI.h"
#include "ChuckinProtoPawn.h"
#include "Classes/Engine/World.h"
#include "TimerManager.h"
#include "Classes/GameFramework/Actor.h"
#include "ChuckinProtoGameMode.h"
#include "ChuckinChickin.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "Public/DrawDebugHelpers.h"

static int32 DebugAIDrawing = 0;
FAutoConsoleVariableRef CVARDebugAIDrawing(
	TEXT("CHUCKIN.DebugAI"),
	DebugAIDrawing,
	TEXT("Draw Debug Lines for AI"),
	ECVF_Cheat);

AChuckinAIController::AChuckinAIController()
{
	AITimeBetweenShotsMin = 3.f;
	AITimeBetweenShotsMax = 7.f;
}


void AChuckinAIController::BeginPlay()
{
	Super::BeginPlay();

	ControlledPawn = Cast<AChuckinAI>(GetPawn());
	PlayerPawn = Cast<AChuckinProtoPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	float AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinAIController::FireAtPlayer, AITimeBetweenShots, true, 3.f);

}

void AChuckinAIController::FireAtPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("AI Pawn: %s, Player Pawn: %s"), *ControlledPawn->GetName(), *PlayerPawn->GetName());

	if (!ProjectileClass) { return; }

	//UGameplayStatics::PlaySoundAtLocation(this, ShootingSoundEffect, GetActorLocation());
	FVector StartLocation = ControlledPawn->GetActorLocation();
	FVector OutLaunchVelocity(0);
	FVector HitLocation = PlayerPawn->GetTargetLocation();
	float LaunchSpeed = 10000.f;

	if (UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0.f,
		0.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
	)
		)
	{
		FVector AimDirection = OutLaunchVelocity.GetSafeNormal();
		FRotator AimAsRotator = AimDirection.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = ControlledPawn;
		SpawnParams.Owner = ControlledPawn;

		if (DebugAIDrawing)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitLocation, FColor::Blue, false, 5.f, 0, 2.f);
		}
		// Look At Player
		FVector NewDirection = HitLocation - StartLocation;
		FRotator LookAtPlayer;
		LookAtPlayer.Yaw = AimAsRotator.Yaw + 90.f;
		LookAtPlayer.Pitch = ControlledPawn->GetActorRotation().Pitch;
		LookAtPlayer.Roll = ControlledPawn->GetActorRotation().Roll;
		if (ControlledPawn)
		{
			ControlledPawn->SetActorRotation(LookAtPlayer);
		}
		

		//UE_LOG(LogTemp, Warning, TEXT("AI Location: %s"), *ControlledPawn->GetActorLocation().ToString());
		AChuckinChickin* Chicken = Cast<AChuckinChickin>(GetWorld()->SpawnActor<AActor>(ProjectileClass, StartLocation, AimAsRotator, SpawnParams));
		//ControlledPawn->MoveIgnoreActorAdd(Chicken);
		if (Chicken)
		{
			Chicken->LaunchProjectile(LaunchSpeed);
		}
		
	}
}
