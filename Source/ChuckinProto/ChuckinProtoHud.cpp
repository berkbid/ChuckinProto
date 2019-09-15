// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ChuckinProtoHud.h"
#include "ChuckinProtoPawn.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"

// Needed for VR Headset
#include "Engine.h"

#define LOCTEXT_NAMESPACE "VehicleHUD"

#ifndef HMD_MODULE_INCLUDED
#define HMD_MODULE_INCLUDED 0
#endif

AChuckinProtoHud::AChuckinProtoHud()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = Font.Object;

	//static ConstructorHelpers::FObjectFinder<UTexture2D> CrossHairTexture(TEXT("/Game/CrosshairTextures/T_Circle_Border_wStroke"));
	//CrosshairTex = CrossHairTexture.Object;
}

void AChuckinProtoHud::DrawHUD()
{
	Super::DrawHUD();

	// draw the crosshair
	if (CrosshairTex)
	{	
		// find center of the Canvas
		const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
		const FVector2D CrosshairDrawPosition((Center.X - 8.f),
			(Center.Y - 8.f));

		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO CROSSHAIR TEXTURE"));
	}


	// Calculate ratio from 720p
	const float HUDXRatio = Canvas->SizeX / 1280.f;
	const float HUDYRatio = Canvas->SizeY / 720.f;

	bool bHMDDeviceActive = false;

	// We dont want the onscreen hud when using a HMD device	
#if HMD_MODULE_INCLUDED
	bHMDDeviceActive = GEngine->IsStereoscopic3D();
#endif // HMD_MODULE_INCLUDED
	if( bHMDDeviceActive == false )
	{
		// Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
		AChuckinProtoPawn* Vehicle = Cast<AChuckinProtoPawn>(GetOwningPawn());
		if ((Vehicle != nullptr) && (Vehicle->bInCarCameraActive == false))
		{
			FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

			// Speed
			FCanvasTextItem SpeedTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 455), Vehicle->SpeedDisplayString, HUDFont, FLinearColor::White);
			SpeedTextItem.Scale = ScaleVec;
			Canvas->DrawItem(SpeedTextItem);

			// Gear
			FCanvasTextItem GearTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 500.f), Vehicle->GearDisplayString, HUDFont, Vehicle->bInReverseGear == false ? Vehicle->GearDisplayColor : Vehicle->GearDisplayReverseColor);
			GearTextItem.Scale = ScaleVec;
			Canvas->DrawItem(GearTextItem);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("HUD DRAWN"));
}

#undef LOCTEXT_NAMESPACE
