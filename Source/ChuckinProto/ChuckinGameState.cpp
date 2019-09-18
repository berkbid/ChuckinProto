// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinGameState.h"

void AChuckinGameState::SetWaveState(EWaveState NewState)
{
	WaveState = NewState;
}

EWaveState AChuckinGameState::GetWaveState() const
{
	return WaveState;
}
