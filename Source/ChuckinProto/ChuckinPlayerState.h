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

	AChuckinPlayerState();

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	void RemoveLife();

	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
	float Lives;

	// Set this here to persist when pawn dies
	bool bShouldCameraBeLocked;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float DefaultNumOfLives;
};
