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
#include "BluePrint/WidgetBlueprintLibrary.h"
#include "ChuckinGameState.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinChickin.h"

AChuckinPlayerController::AChuckinPlayerController()
{
	CrossHairXLocation = 0.5f;
	CrossHairYLocation = 0.5f;

	// 1million centimeter range
	LineTraceRange = 1000000.f;

	GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &AChuckinPlayerController::GetSizeXY);
	SetScreenLocation();

	TimeBetweenShots = 0.5f;
	RateOfFire = 200.f;
	bShouldCameraBeLocked = false;
}

// Override this, this allows player controller to setup Pawn values RIGHT when it is possessed.
void AChuckinPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	AChuckinProtoPawn* CPPS = Cast<AChuckinProtoPawn>(InPawn);
	if (CPPS)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Possessing Pawn: %s"), *CPPS->GetName());
		CPPS->bIsCameraLocked = bShouldCameraBeLocked;
	}

}

void AChuckinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Game Info widget to viewport
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
}

class AChuckinProtoPawn* AChuckinPlayerController::GetCar() const
{
	return Cast<AChuckinProtoPawn>(GetPawn());
}

void AChuckinPlayerController::FireAtCrosshair()
{
	if (!GetCar()) { return; }

	GetCar()->FireAt();
}

void AChuckinPlayerController::GetSizeXY(FViewport* ViewPort, uint32 val)
{
	SetScreenLocation();
}


void AChuckinPlayerController::StartFireTarget()
{
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

void AChuckinPlayerController::ResumePlay()
{
	if (UGameplayStatics::SetGamePaused(this, false))
	{
		if (MyPauseMenu)
		{
			MyPauseMenu->RemoveFromViewport();
		}
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		bShowMouseCursor = false;
	}
	else
	{
		if (MyGameState)
		{
			MyGameState->RemoveFromViewport();
		}
	}
}

void AChuckinPlayerController::RestartLevelNew()
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	bShowMouseCursor = false;
	RestartLevel();
}


void AChuckinPlayerController::PauseGame()
{
	// Try to pause the game and if it succeeds, display the UI
	if (UGameplayStatics::SetGamePaused(this, true))
	{
		// Add Game Info widget to viewport
		if (wPauseMenu)
		{
			MyPauseMenu = CreateWidget<UUserWidget>(this, wPauseMenu);

			if (MyPauseMenu)
			{
				MyPauseMenu->AddToViewport();
				//UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, MyPauseMenu);
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MyPauseMenu);
				bShowMouseCursor = true;
			}
		}
	}
}

void AChuckinPlayerController::ShowGameState()
{
	// Add Game Info widget to viewport
	if (wGameState)
	{
		MyGameState = CreateWidget<UUserWidget>(this, wGameState);

		if (MyGameState)
		{
			MyGameState->AddToViewport();
		}
	}
}

void  AChuckinPlayerController::ShowGameOverMenu() 
{
	if (wGameOverMenu) 
	{
		MyGameOverMenu = CreateWidget<UUserWidget>(this, wGameOverMenu);
		if (MyGameOverMenu)
		{
			MyGameOverMenu->AddToViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MyGameOverMenu);
			bShowMouseCursor = true;
		}
	}
}

void AChuckinPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// set up gameplay key bindings
	//check(InputComponent);
	//InputComponent->BindAction
	InputComponent->BindAction("FireTarget", IE_Pressed, this, &AChuckinPlayerController::StartFireTarget);
	InputComponent->BindAction("FireTarget", IE_Released, this, &AChuckinPlayerController::StopFireTarget);

	InputComponent->BindAction("RestartPlayer", IE_Pressed, this, &AChuckinPlayerController::RestartLevelNew);
	InputComponent->BindAction("Pause", IE_Pressed, this, &AChuckinPlayerController::PauseGame);

}