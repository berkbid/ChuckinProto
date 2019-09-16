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

		AChuckinProtoPawn* KillerPawn = Cast<AChuckinProtoPawn>(KillerController->GetPawn());
		if (KillerPawn)
		{
			AChuckinPlayerState* KillerPS = Cast<AChuckinPlayerState>(KillerPawn->GetPlayerState());

			if (KillerPS)
			{
				KillerPS->AddScore(10.f);
			}
		}
		APawn* tempP = Cast<APawn>(VictimActor);
		if (tempP)
		{
			AChuckinPlayerState* VictimPS = Cast<AChuckinPlayerState>(tempP->GetPlayerState());

			// This means the victim has AChuckinPlayerState thus must be a player not an AI
			if (VictimPS)
			{
				VictimPS->RemoveLife();
				PrepareForSpawn();
			}
			
		}

		
	}
	
}

void AChuckinProtoGameMode::StartPlay()
{
	Super::StartPlay();

}

void AChuckinProtoGameMode::PrepareForSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("PrepareForStart()"))
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AChuckinProtoGameMode::RestartDeadPlayer, TimeBeforeStart, false, TimeBeforeStart);
	
	AChuckinPlayerController* PC = Cast<AChuckinPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->ShowGameState();
	}
}

void AChuckinProtoGameMode::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Wave"))
}



void AChuckinProtoGameMode::RestartDeadPlayer()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_NextWaveStart);
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && (PC->GetPawn() == nullptr))
	{
		AChuckinPlayerController* CPC = Cast<AChuckinPlayerController>(PC);
		if (CPC)
		{
			CPC->ResumePlay();
		}

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
