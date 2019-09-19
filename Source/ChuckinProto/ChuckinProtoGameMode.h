// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChuckinProtoGameMode.generated.h"

enum class EWaveState : uint8;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);  // Killed actor, Killer actor,

UCLASS(MinimalAPI)
class AChuckinProtoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChuckinProtoGameMode();

	//virtual void Tick(float DeltaSeconds) override;

	virtual void StartPlay() override;

	void PrepareForSpawn();

	// Event for blueprint to hook on when actor gets killed
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleActorKilled(AActor* VictimActor, AActor* KillerActor, AController* KillerController);

	// Blueprint can use to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnAITruck();

	void CheckAIAlive();

	void RestartDeadPlayer();

	// Start spawning bots
	void StartWave();

	// Stop spawning bots
	void EndWave();

	void GameOver();

	void GameWon();

	// Set timer for next startwave
	void PrepareForNextWave();

	void SpawnAITimerElapsed();

	void SetWaveState(EWaveState NewState);

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	FTimerHandle TimerHandle_AISpawn;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBeforeStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	int32 NumAIAddedPerWave;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int32 NumberOfWavesForVictory;

	// How many AI we are spawning this wave
	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	int32 NumberOfAIToSpawn;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	int32 WaveNumber;
};



