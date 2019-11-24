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
#include "Uobject/UObjectBase.h"

static int32 DebugAIDrawing = 0;
FAutoConsoleVariableRef CVARDebugAIDrawing(
	TEXT("CHUCKIN.DebugAI"),
	DebugAIDrawing,
	TEXT("Draw Debug Lines for AI"),
	ECVF_Cheat);

AChuckinAIController::AChuckinAIController()
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Default values for minimum and maximum seconds range for AI to fire
	AITimeBetweenShotsMin = 3.f;
	AITimeBetweenShotsMax = 7.f;
	AIDistanceToPlayer = 1000.f;

	// This makes the AIControllers persistent, and logic for starting/stopping the actions are already setup in
	// OnPossess and OnUnPossess override methods
	//bWantsPlayerState = true;

}


void AChuckinAIController::Tick(float Delta)
{
	// Need to halt tick function upon Pawn Destruction
	Super::Tick(Delta);

	FVector EndLocation(0);

	if (PlayerPawn)
	{
		// Hopefully this fixes progressing further in this Tick function and getting stuck on PlayerPawn->GetActorLocation() where the Pawn reference
		// Must somehow become null during that function call, leading to the game crashing.
		if (PlayerPawn->IsPendingKill()) { return; }

		// For some reason, UE4 can crash here because PlayerPawn reference isn't valid? When player dies... no respawning screen, just crash
		EndLocation = PlayerPawn->GetActorLocation();
	}

	if (ControlledPawn)
	{
		// Face AI pawn towards target at all times
		FVector StartLocation = ControlledPawn->GetActorLocation();
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);
		// Manually rotate pawn since it's original forward direction is wrong
		LookRotation.Yaw += 90.f;
		ControlledPawn->FaceRotation(LookRotation, 0.f);
	}
	
}


// Hook onto when the player is destroyed event
void AChuckinAIController::HandlePlayerDestroyed(AActor* Act)
{
	// Set this reference to nullptr so this AI needs a new reference and will run the code inside GetPlayerReference()
	PlayerPawn = nullptr;

	// Stop AI movement when player has died
	StopMovement();

	// Setup random delays to start performing move/shoot again after player has died
	float SpawnDelay = FMath::RandRange(5.f, 8.f);
	float ShootDelay = FMath::RandRange(3.f, 6.f);

	// Clear timer for AI moving towards player, wait 8seconds(3 for player respawn, 5 to let player breath) before looping MoveTowardsPlayer every 5 seconds again
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenMoveTo);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenMoveTo, this, &AChuckinAIController::MoveTowardsPlayer, 5.f, true, SpawnDelay);

	// Clear timer for AI shooting player, wait 8seconds before looping FireAtPlayer again
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinAIController::FireAtPlayer, AITimeBetweenShots, true, ShootDelay + AITimeBetweenShots);

}

void AChuckinAIController::StopAllActions()
{
	SetActorTickEnabled(false);

	// Stop AI movement when player has died
	StopMovement();

	// Clear active timers
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenMoveTo);
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AChuckinAIController::StartAllActions()
{
	// Grab reference to the Pawn this AI Controller is possessing
	GetControlledPawnReference();

	// Grab reference to the Pawn of the human player, to use for location etc.
	GetPlayerReference();

	//bCanTick = true;
	SetActorTickEnabled(true);
	// Create random amount of time to fire chicken
	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);

	// Every 5 seconds Move towards player
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenMoveTo, this, &AChuckinAIController::MoveTowardsPlayer, 5.f, true, 2.f);

	// Call FireAtPlayer() function looping
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinAIController::FireAtPlayer, AITimeBetweenShots, true, AITimeBetweenShots);
}

void AChuckinAIController::BeginPlay()
{
	Super::BeginPlay();

}

// We Begin Inactive State when AController::PawnPendingDestroy() is called from APawn::Destroyed() into APawn::DetachFromControllerPendingDestroy()
// This State gets set, AFTER Pawn->UnPossessed() AND Controller->UnPossessed() is called
void AChuckinAIController::BeginInactiveState()
{
	//UE_LOG(LogTemp, Warning, TEXT("Beginning Inactive State"));
}

// THIS WILL ONLY BE CALLED IF (In OnPossess() of AIController.cpp) bWantsPlayerState = true !! NEEDS PLAYER STATE FOR THIS
// Also requires current state to be NAME_Inactive before OnPossess is called... which it isn't
void AChuckinAIController::EndInactiveState()
{
	//UE_LOG(LogTemp, Warning, TEXT("Ending Inactive State"));
}

// STATE GETS SET TO "PLAYING" IN AIController.cpp
void AChuckinAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	StartAllActions();
}

void AChuckinAIController::OnUnPossess()
{
	Super::OnUnPossess();

	StopAllActions();
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

		// Move to target actor using navigation
		MoveToActor(PlayerPawn, AIDistanceToPlayer, true, true, false);
	}
}

void AChuckinAIController::GetPlayerReference()
{
	// If we don't have a reference to player, get it and hook on the destroy event
	if (!PlayerPawn)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			APawn* PPawn = PC->GetPawn();
			if (PPawn)
			{
				// Hook to destroyed event of pawn since this is first time getting reference to it
				PPawn->OnDestroyed.AddDynamic(this, &AChuckinAIController::HandlePlayerDestroyed);

				// Set PlayerPawn pointer to the player
				PlayerPawn = Cast<AChuckinProtoPawn>(PPawn);
			}
		}
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

