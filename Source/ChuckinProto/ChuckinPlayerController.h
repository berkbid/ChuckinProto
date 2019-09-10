// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChuckinPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CHUCKINPROTO_API AChuckinPlayerController : public APlayerController
{
	GENERATED_BODY()
	


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wCrosshair;

	UUserWidget* MyCrosshair;

	virtual void BeginPlay() override;
};
