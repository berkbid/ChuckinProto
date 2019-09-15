// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ChuckinProtoGameMode.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinProtoHud.h"
#include "ChuckinGameState.h"
#include "Public/TimerManager.h"
#include "Engine/World.h"

AChuckinProtoGameMode::AChuckinProtoGameMode()
{
	TimeBeforeStart = 3.f;

	DefaultPawnClass = AChuckinProtoPawn::StaticClass();
	HUDClass = AChuckinProtoHud::StaticClass();
	GameStateClass = AChuckinGameState::StaticClass();
}

void AChuckinProtoGameMode::StartPlay()
{
	Super::StartPlay();

	//PrepareForStart();
}

void AChuckinProtoGameMode::PrepareForStart()
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
