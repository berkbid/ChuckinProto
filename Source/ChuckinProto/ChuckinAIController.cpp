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


void AChuckinAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AChuckinAIController::BeginPlay()
{
	Super::BeginPlay();

	// Grab reference to the Pawn this AI Controller is possessing
	ControlledPawn = Cast<AChuckinAI>(GetPawn());

	// Grab reference to the Pawn of the human player, to use for location etc.
	PlayerPawn = Cast<AChuckinProtoPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// Create random amount of time to fire chicken
	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);

	NextPathPoint = GetNextPathPoint();

	// Call FireAtPlayer() function looping
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinAIController::FireAtPlayer, AITimeBetweenShots, true, AITimeBetweenShots);

	

}

void AChuckinAIController::FireAtPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("AI Pawn: %s, Player Pawn: %s"), *ControlledPawn->GetName(), *PlayerPawn->GetName());

	if (!ProjectileClass) { return; }

	// Look for player every time AI fires? Seems excessive.
	PlayerPawn = Cast<AChuckinProtoPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
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

		// This seems to have no effect to the chicken launchspeed.
		// TODO: Figure out how to change chicken speed on launch
		if (Chicken)
		{
			Chicken->LaunchProjectile(LaunchSpeed);
		}
		
	}

	AITimeBetweenShots = FMath::RandRange(AITimeBetweenShotsMin, AITimeBetweenShotsMax);
}

FVector AChuckinAIController::GetNextPathPoint()
{
	//if (PlayerPawn)
	//{
	//	AActor* PlayerActor = Cast<AActor>(PlayerPawn);
	//	if (PlayerActor && ControlledPawn)
	//	{
	//		//UNavigationSystemV1::SimpleMoveToActor(this, PlayerActor);
	//		UAIBlueprintHelperLibrary::SimpleMoveToActor(this, PlayerActor);
	//		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, ControlledPawn->GetActorLocation(), PlayerActor);

	//		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &AChuckinAIController::RefreshPath, 5.f, false);

	//		if (NavPath)
	//		{
	//			if (NavPath->PathPoints.Num() > 1)
	//			{
	//
	//				
	//				return NavPath->PathPoints[1];
	//			}
	//		}

	//	}
	//}

	// If player has respawned, need to make new reference to player pawn
	if (!PlayerPawn)
	{
		PlayerPawn = Cast<AChuckinProtoPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}

	if (PlayerPawn)
	{
		AActor* PlayerActor = Cast<AActor>(PlayerPawn);
		if (PlayerActor)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Moving to actor: %s, location: %s"), *PlayerActor->GetName(), *PlayerActor->GetActorLocation().ToString());
			UAIBlueprintHelperLibrary::SimpleMoveToActor(this, PlayerActor);
			GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &AChuckinAIController::RefreshPath, 5.f, false);
		}
		return PlayerPawn->GetActorLocation();
	}
	else
	{
		return FVector(0);
	}
}

void AChuckinAIController::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}
