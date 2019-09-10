// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckinPlayerController.h"
#include "Blueprint/UserWidget.h"

void AChuckinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (wCrosshair)
	{
		MyCrosshair = CreateWidget<UUserWidget>(this, wCrosshair);

		if (MyCrosshair)
		{
			MyCrosshair->AddToViewport();
		}
	}
}
