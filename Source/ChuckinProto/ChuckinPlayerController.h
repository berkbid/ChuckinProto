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
	AChuckinPlayerController();

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wCrosshair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wGameState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wGameOverMenu;

	UUserWidget* MyCrosshair;

	UUserWidget* MyPauseMenu;

	UUserWidget* MyGameState;

	UUserWidget* MyGameOverMenu;

	virtual void BeginPlay() override;

	void GetSizeXY(FViewport* ViewPort, uint32 val);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFireTarget();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFireTarget();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResumePlay();

	void RestartPlayerNew();

	void PauseGame();

	void ShowGameState();

	void ShowGameOverMenu();

	//void ShowGameOverState();

	FTimerHandle TimerHandle_TimeBetweenShots;

	// Derived from RateOfFire
	float TimeBetweenShots;

	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "Chicken")
	float RateOfFire;


protected:
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrossHairXLocation;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrossHairYLocation;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float LineTraceRange;

	FVector2D ScreenLocation;

	void SetScreenLocation();

	class AChuckinProtoPawn* GetCar() const;

	void FireAtCrosshair();

	
};
