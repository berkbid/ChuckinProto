// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ChuckinGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaitingToComplete,
	GameOver,
	WaveComplete

};


/**
 * 
 */
UCLASS()
class CHUCKINPROTO_API AChuckinGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;
	
public:
	void SetWaveState(EWaveState NewState);
};
