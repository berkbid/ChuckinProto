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
#include "ChuckinAI.h"
#include "ChuckinHealthComponent.h"

AChuckinProtoGameMode::AChuckinProtoGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set tick interval to 1 second
	PrimaryActorTick.TickInterval = 1.f;

	TimeBeforeStart = 3.f;
	NumberOfAIToSpawn = 3;
	WaveNumber = 0;
	TimeBetweenWaves = 2.f;
	NumAIAddedPerWave = 10;

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

void AChuckinProtoGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Instead of checking every second, could bind event to bot being destroyed and hook to it
	CheckAIAlive();

	// Could also bind to when players die instead of check every second
	//CheckAnyPlayerAlive();
}

void AChuckinProtoGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AChuckinProtoGameMode::BeginPlay()
{
	Super::BeginPlay();

	// This is custom event we call from healthcomponent when an actor reaches 0 health
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
		APawn* VictimPawn = Cast<APawn>(VictimActor);
		if (VictimPawn)
		{
			AChuckinPlayerState* VictimPS = Cast<AChuckinPlayerState>(VictimPawn->GetPlayerState());

			// This means the victim has AChuckinPlayerState thus must be a player not an AI
			if (VictimPS)
			{
				VictimPS->RemoveLife();
				if (VictimPS->Lives <= 0)
				{
					GameOver();
				}
				else
				{
					PrepareForSpawn();
				}
				
			}
			
		}
	}
	// Destroy the victim actor here after everything has been taken care of
	if (VictimActor)
	{
		VictimActor->Destroy();
	}

}

void AChuckinProtoGameMode::PrepareForSpawn()
{
	SetWaveState(EWaveState::Respawning);
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AChuckinProtoGameMode::RestartDeadPlayer, TimeBeforeStart, false, TimeBeforeStart);
	AChuckinPlayerController* PC = Cast<AChuckinPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->ShowGameState();
	}
}

void AChuckinProtoGameMode::StartWave()
{
	WaveNumber++;
	NumberOfAIToSpawn = NumAIAddedPerWave * WaveNumber;
	UE_LOG(LogTemp, Warning, TEXT("Starting Wave: Spawning %d AI TRUCKS"), NumberOfAIToSpawn);

	GetWorldTimerManager().SetTimer(TimerHandle_AISpawn, this, &AChuckinProtoGameMode::SpawnAITimerElapsed, 0.5f, true, 0.f);
}



void AChuckinProtoGameMode::EndWave()
{
	// Stop spawming AI
	GetWorldTimerManager().ClearTimer(TimerHandle_AISpawn);
}

void AChuckinProtoGameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);

	AChuckinPlayerController* PC = Cast<AChuckinPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		// Disable player controller input upon game over so they cannot shoot or pause or restart level
		PC->DisableInput(PC);
		PC->ShowGameOverMenu();
	}
}

void AChuckinProtoGameMode::PrepareForNextWave()
{
	UE_LOG(LogTemp, Warning, TEXT("PrepareForNextWave()"))
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AChuckinProtoGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::WaitingToStart);

}

void AChuckinProtoGameMode::SpawnAITimerElapsed()
{
	// Blueprint implements this function
	SpawnAITruck();

	NumberOfAIToSpawn--;
	if (NumberOfAIToSpawn <= 0)
	{
		EndWave();
	}
}

void AChuckinProtoGameMode::CheckAIAlive()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	// Never want to prepare for next wave if already preparing or if still have bots to spawn
	if (NumberOfAIToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		AChuckinAI* AITruck = Cast<AChuckinAI>(TestPawn);

		if (AITruck == nullptr)
		{
			continue;
		}

		UChuckinHealthComponent* HealthComp = Cast<UChuckinHealthComponent>(AITruck->GetComponentByClass(UChuckinHealthComponent::StaticClass()));

		if (HealthComp && HealthComp->GetHealth() > 0.f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	// This will fail if we found an alive bot in the forloop above where this boolean is set to true
	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
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
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}
