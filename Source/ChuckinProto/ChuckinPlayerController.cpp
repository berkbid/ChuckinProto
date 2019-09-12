// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ChuckinProtoPawn.h"
#include "Engine/Public/UnrealClient.h"
#include "Engine.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "ChuckinProtoGameMode.h"

AChuckinPlayerController::AChuckinPlayerController()
{
	CrossHairXLocation = 0.5f;
	CrossHairYLocation = 0.5f;

	// 1millino centimeter range
	LineTraceRange = 1000000.f;

	GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &AChuckinPlayerController::GetSizeXY);
	SetScreenLocation();

	TimeBetweenShots = 0.5f;
	RateOfFire = 200.f;
}

void AChuckinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (wCrosshair)
	{
		MyCrosshair = CreateWidget<UUserWidget>(this, wCrosshair);

		if (MyCrosshair)
		{
			MyCrosshair->AddToViewport();
		}
	}
	SetScreenLocation();

	TimeBetweenShots = 60.f / RateOfFire;
}

// Get world location of linetrace through crosshair, true if hits landscape
void AChuckinPlayerController::SetScreenLocation()
{
	// Get viewport size
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Maybe don't set these if the value is 0?
	if ((ViewportSizeY && ViewportSizeX) != 0)
	{
		ScreenLocation.X = ViewportSizeX * CrossHairXLocation;
		ScreenLocation.Y = ViewportSizeY * CrossHairYLocation;
	}

	//UE_LOG(LogTemp, Warning, TEXT("ScreenLocation X: %s"), *FString::SanitizeFloat(ScreenLocation.X));
	//UE_LOG(LogTemp, Warning, TEXT("ScreenLocation Y: %s"), *FString::SanitizeFloat(ScreenLocation.Y));
	UE_LOG(LogTemp, Warning, TEXT("ScreenLocation: %s"), *ScreenLocation.ToString());
}

class AChuckinProtoPawn* AChuckinPlayerController::GetCar() const
{
	return Cast<AChuckinProtoPawn>(GetPawn());
}

void AChuckinPlayerController::FireAtCrosshair()
{
	//UE_LOG(LogTemp, Warning, TEXT("PC: No Owned Car"));
	if (!GetCar()) { return; }
	//UE_LOG(LogTemp, Warning, TEXT("PC: Found Owned Car"));
	FVector CameraLocation;
	FVector LookDirection;

	if (DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, CameraLocation, LookDirection))
	{
		FVector HitLocation;
		//UE_LOG(LogTemp, Warning, TEXT("Look Direction: %s"), *WorldDirection.ToString());
		FHitResult TraceHitResult;
		FVector StartLocation = PlayerCameraManager->GetCameraLocation();
		FVector EndLocation = StartLocation + LookDirection * LineTraceRange;

		// Ignoring the controlled car from line trace collision for when the crosshair is on the Car
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetCar());

		if (
			GetWorld()->LineTraceSingleByChannel(
				TraceHitResult,
				StartLocation,
				EndLocation,
				ECollisionChannel::ECC_Visibility,
				CollisionParams
			)
			)
		{
			if (Cast<AActor>(TraceHitResult.Actor))
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *(TraceHitResult.Actor->GetName()));
			}
			
			HitLocation = TraceHitResult.Location;

			// Only call AimAt if LineTrace succeeds
			GetCar()->FireAt(HitLocation);
		}
		else
		{
			HitLocation = FVector(0);
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: NONE"));
		}


		//UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *HitLocation.ToString());

	}
}

void AChuckinPlayerController::GetSizeXY(FViewport* ViewPort, uint32 val)
{
	SetScreenLocation();
}


void AChuckinPlayerController::StartFireTarget()
{
	//UE_LOG(LogTemp, Warning, TEXT("PC::StartFire"));
	// This makes it so you cannot single fire faster than you can automatic fire
	// Use greatest of first or 2nd value, clamps between 0 and other value
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AChuckinPlayerController::FireAtCrosshair, TimeBetweenShots, true, FirstDelay);
}

void AChuckinPlayerController::StopFireTarget()
{
	//UE_LOG(LogTemp, Warning, TEXT("PC::StopFire"));
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AChuckinPlayerController::RestartPlayerNew()
{
	//AChuckinProtoGameMode* GM = Cast<AChuckinProtoGameMode>(GetWorld()->GetAuthGameMode());
	//if (GM)
	//{
	//	GM->RestartPlayer(this);
	//}
	
	RestartLevel();
	//RestartPlayer();
	
}


void AChuckinPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// set up gameplay key bindings
	//check(InputComponent);
	//InputComponent->BindAction
	InputComponent->BindAction("FireTarget", IE_Pressed, this, &AChuckinPlayerController::StartFireTarget);
	InputComponent->BindAction("FireTarget", IE_Released, this, &AChuckinPlayerController::StopFireTarget);

	InputComponent->BindAction("RestartPlayer", IE_Pressed, this, &AChuckinPlayerController::RestartPlayerNew);

}