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
#include "Public/NavigationSystem.h"
#include "Public/NavigationPath.h"
#include "GameFramework/Actor.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"

static int32 DebugAIDrawing = 0;
FAutoConsoleVariableRef CVARDebugAIDrawing(
	TEXT("CHUCKIN.DebugAI"),
	DebugAIDrawing,
	TEXT("Draw Debug Lines for AI"),
	ECVF_Cheat);

AChuckinAIController::AChuckinAIController()
{
	// Default values for minimum and maximum seconds range for AI to fire
	AITimeBetweenShotsMin = 3.f;
	AITimeBetweenShotsMax = 7.f;
}


void AChuckinAIController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (ControlledPawn && PlayerPawn)
	{
		//FRotator LookRotation(0);
		//LookRotation.Yaw = UKismetMathLibrary::FindLookAtRotation(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation()).Yaw + 90.f;
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
		LookRotation.Yaw += 90.f;
		ControlledPawn->FaceRotation(LookRotation, 0.f);
	}
	

}

// Event overriden from parent, when AI completes movement to a location
void AChuckinAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// Wait 2 seconds, then re-target player and move towards player again with a 5 second looping call
	//GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenMoveTo);
	//GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenMoveTo, this, &AChuckinAIController::MoveTowardsPlayer, 5.f, true, 2.f);
}

void AChuckinAIController::BeginPlay()
{
	Super::BeginPlay();

	// Grab reference to the Pawn this AI Controller is possessing
	ControlledPawn = Cast<AChuckinAI>(GetPawn());

	// Grab reference to the Pawn of the human player, to use for location etc.
	GetPlayerReference();

	// Create random amount of time to fire chicken
	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);

	//NextPathPoint = GetNextPathPoint();
	MoveTowardsPlayer();
	

	// Call FireAtPlayer() function looping
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinAIController::FireAtPlayer, AITimeBetweenShots, true, AITimeBetweenShots);

	// Every 5 seconds Move towards player
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenMoveTo, this, &AChuckinAIController::MoveTowardsPlayer, 5.f, true, 5.f);

}

void AChuckinAIController::FireAtPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("AI Pawn: %s, Player Pawn: %s"), *ControlledPawn->GetName(), *PlayerPawn->GetName());
	if (!ProjectileClass) { return; }

	// Look for player every time AI fires? Seems excessive
	GetPlayerReference();
	if (!PlayerPawn) { return; }

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
		
		//UE_LOG(LogTemp, Warning, TEXT("AI Location: %s"), *ControlledPawn->GetActorLocation().ToString());
		AChuckinChickin* Chicken = Cast<AChuckinChickin>(GetWorld()->SpawnActor<AActor>(ProjectileClass, StartLocation, AimAsRotator, SpawnParams));
		//ControlledPawn->MoveIgnoreActorAdd(Chicken);

		// This seems to have no effect to the chicken launchspeed.
		// TODO: Figure out how to change chicken speed on launch
		if (Chicken)
		{
			Chicken->LaunchProjectile(LaunchSpeed);
		}
		
	}

	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);
}

void AChuckinAIController::MoveTowardsPlayer()
{
	// If we don't have a reference to the player pawn because it got destroyed, try and find a new reference.
	GetPlayerReference();

	// If we have valid references to the playerpawn and the AI controlled pawn, then lets MoveToActor
	if (ControlledPawn && PlayerPawn)
	{
		// Must set focus to set focal point for AI to face towards focal point
		//SetFocus(PlayerPawn);
		// Flip AI car upright before moving towards player
		FRotator NewRotation(0);
		NewRotation.Yaw = ControlledPawn->GetActorRotation().Yaw;
		ControlledPawn->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);

		// Move towards actor
		//MoveToActor(PlayerPawn, 650.f, true, true, true);

		// Should Strafing be false:
		MoveToActor(PlayerPawn, 650.f, true, true, false);
	}
}

void AChuckinAIController::GetPlayerReference()
{

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PlayerPawn = Cast<AChuckinProtoPawn>(PC->GetPawn());
	}

}

