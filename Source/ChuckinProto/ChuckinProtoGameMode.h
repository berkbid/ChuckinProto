// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChuckinProtoGameMode.generated.h"

enum class EWaveState : uint8;

UCLASS(MinimalAPI)
class AChuckinProtoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChuckinProtoGameMode();

	virtual void StartPlay() override;

	
	void PrepareForStart();

protected:
	void RestartDeadPlayer();

	// Start spawning bots
	void StartWave();

	

	void SetWaveState(EWaveState NewState);

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBeforeStart;
};



