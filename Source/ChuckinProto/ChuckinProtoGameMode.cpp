// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ChuckinProtoGameMode.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinProtoHud.h"
#include "ChuckinGameState.h"
#include "Public/TimerManager.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "ChuckinGameState.h"
#include "ChuckinPlayerState.h"
#include "ChuckinPlayerController.h"
#include "GameFramework/Controller.h"

AChuckinProtoGameMode::AChuckinProtoGameMode()
{
	TimeBeforeStart = 3.f;

	// Set Default Pawn Class From Blueprint
	static ConstructorHelpers::FClassFinder<AChuckinProtoPawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_CarPawn"));
	if (PlayerPawnClassFinder.Succeeded())
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;
	}

	// Set Default GameState Class From Blueprint
	static ConstructorHelpers::FClassFinder<AChuckinGameState> GameStateClassFinder(TEXT("/Game/Blueprints/BP_GameState"));
	if (GameStateClassFinder.Succeeded())
	{
		GameStateClass = GameStateClassFinder.Class;
	}

	// Set Default Player Controller Class From Blueprint
	static ConstructorHelpers::FClassFinder<AChuckinPlayerController> PlayerControllerClassFinder(TEXT("/Game/Blueprints/BP_PlayerController"));
	if (PlayerControllerClassFinder.Succeeded())
	{
		PlayerControllerClass = PlayerControllerClassFinder.Class;
	}

	// Set Default Player State Class From Blueprint
	static ConstructorHelpers::FClassFinder<AChuckinPlayerState> PlayerStateClassFinder(TEXT("/Game/Blueprints/BP_PlayerState"));
	if (PlayerStateClassFinder.Succeeded())
	{
		PlayerStateClass = PlayerStateClassFinder.Class;
	}

	// Set Default HUD Class From Blueprint
	static ConstructorHelpers::FClassFinder<AChuckinProtoHud> PlayerHUDClassFinder(TEXT("/Game/Blueprints/BP_HUD"));
	if (PlayerHUDClassFinder.Succeeded())
	{
		HUDClass = PlayerHUDClassFinder.Class;
	}


	//HUDClass = AChuckinProtoHud::StaticClass();
	
}

void AChuckinProtoGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnActorKilled.AddDynamic(this, &AChuckinProtoGameMode::HandleActorKilled);

}

void AChuckinProtoGameMode::HandleActorKilled(AActor* VictimActor, AActor* KillerActor, AController* KillerController)
{
	if (VictimActor && KillerActor && KillerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Died: %s, Killed by : %s"), *VictimActor->GetName(), *KillerActor->Instigator->GetName());
		AChuckinPlayerState* PS = Cast<AChuckinPlayerState>(KillerController->GetPawn()->GetPlayerState());
		if (PS)
		{
			PS->AddScore(10.f);
		}
		
	}
	

}

void AChuckinProtoGameMode::StartPlay()
{
	Super::StartPlay();

	//PrepareForStart();
}

void AChuckinProtoGameMode::PrepareForSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("PrepareForStart()"))
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AChuckinProtoGameMode::RestartDeadPlayer, TimeBeforeStart, false, TimeBeforeStart);
}

void AChuckinProtoGameMode::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Wave"))
}



void AChuckinProtoGameMode::RestartDeadPlayer()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && (PC->GetPawn() == nullptr))
	{
		RestartPlayer(PC);
	}
	
}

void AChuckinProtoGameMode::SetWaveState(EWaveState NewState)
{
	AChuckinGameState* GS = GetGameState<AChuckinGameState>();
	if (GS)
	{
		GS->SetWaveState(NewState);
	}
}
