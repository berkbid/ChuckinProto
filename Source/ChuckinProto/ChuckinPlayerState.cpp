// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPlayerState.h"


AChuckinPlayerState::AChuckinPlayerState()
{
	DefaultNumOfLives = 3;
	Lives = DefaultNumOfLives;
	bShouldCameraBeLocked = false;
}
// Allows blueprint to manipulate "Score" since it is marked BlueprintReadOnly in PlayerState.h
void AChuckinPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
}

void AChuckinPlayerState::RemoveLife()
{
	Lives = FMath::Clamp(Lives - 1.f, 0.f, DefaultNumOfLives);
}