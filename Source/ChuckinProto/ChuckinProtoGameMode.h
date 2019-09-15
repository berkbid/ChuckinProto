// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChuckinProtoGameMode.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, class AController*, KillerController);  // Killed actor, Killer actor,

UCLASS(MinimalAPI)
class AChuckinProtoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChuckinProtoGameMode();

	virtual void StartPlay() override;

	void PrepareForSpawn();

	// Event for blueprint to hook on when actor gets killed
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleActorKilled(AActor* VictimActor, AActor* KillerActor, class AController* KillerController);


	void RestartDeadPlayer();

	// Start spawning bots
	void StartWave();

	void SetWaveState(EWaveState NewState);

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBeforeStart;
};



