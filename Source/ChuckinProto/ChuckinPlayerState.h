// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ChuckinPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CHUCKINPROTO_API AChuckinPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);
};
