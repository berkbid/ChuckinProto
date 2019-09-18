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
	AIDistanceToPlayer = 1000.f;
}


void AChuckinAIController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (ControlledPawn)
	{
		FVector StartLocation = ControlledPawn->GetActorLocation();
		FVector EndLocation(0);

		if (PlayerPawn)
		{
			// For some reason, UE4 can crash here because PlayerPawn reference isn't valid? When player dies... no respawning screen, just crash
			EndLocation = PlayerPawn->GetActorLocation();
		}

		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);
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
	GetControlledPawnReference();

	// Grab reference to the Pawn of the human player, to use for location etc.
	GetPlayerReference();

	// Create random amount of time to fire chicken
	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);

	//NextPathPoint = GetNextPathPoint();
	//MoveTowardsPlayer();

	// Every 5 seconds Move towards player
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenMoveTo, this, &AChuckinAIController::MoveTowardsPlayer, 5.f, true, 2.f);

	// Call FireAtPlayer() function looping
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinAIController::FireAtPlayer, AITimeBetweenShots, true, AITimeBetweenShots);

}

void AChuckinAIController::FireAtPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("AI Pawn: %s, Player Pawn: %s"), *ControlledPawn->GetName(), *PlayerPawn->GetName());
	if (!ProjectileClass) { return; }

	// Look for player every time AI fires? Seems excessive
	GetPlayerReference();
	GetControlledPawnReference();
	if (!PlayerPawn) { return; }
	if (!ControlledPawn) { return; }
	
	FVector StartLocation = ControlledPawn->GetActorLocation();

	FVector HitLocation = PlayerPawn->GetTargetLocation();
	float LaunchSpeed = 10000.f;

	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, HitLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = ControlledPawn;
	SpawnParams.Owner = ControlledPawn;

	//UE_LOG(LogTemp, Warning, TEXT("AI Location: %s"), *ControlledPawn->GetActorLocation().ToString());
	AChuckinChickin* Chicken = Cast<AChuckinChickin>(GetWorld()->SpawnActor<AActor>(ProjectileClass, StartLocation, LookRotation, SpawnParams));
	if (Chicken)
	{
		Chicken->LaunchProjectile(LaunchSpeed);
	}
	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);
}

void AChuckinAIController::MoveTowardsPlayer()
{
	// If we don't have a reference to the player pawn because it got destroyed, try and find a new reference.
	GetPlayerReference();
	GetControlledPawnReference();

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
		MoveToActor(PlayerPawn, AIDistanceToPlayer, true, true, false);
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
void AChuckinAIController::GetControlledPawnReference()
{
	APawn* PlayerP = GetPawn();
	if (PlayerP)
	{
		ControlledPawn = Cast<AChuckinAI>(PlayerP);
	}
	
}

