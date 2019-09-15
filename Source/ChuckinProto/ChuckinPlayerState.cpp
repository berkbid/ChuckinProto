// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPlayerState.h"

// Allows blueprint to manipulate "Score" since it is marked BlueprintReadOnly in PlayerState.h
void AChuckinPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
}