// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ChuckinProtoHud.generated.h"

class UTexture2D;

UCLASS(config = Game)
class AChuckinProtoHud : public AHUD
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Textures")
	UTexture2D* CrosshairTex;



public:
	AChuckinProtoHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface


};
