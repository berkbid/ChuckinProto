// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ChuckinProtoGameMode.h"
#include "ChuckinProtoPawn.h"
#include "ChuckinProtoHud.h"

AChuckinProtoGameMode::AChuckinProtoGameMode()
{
	DefaultPawnClass = AChuckinProtoPawn::StaticClass();
	HUDClass = AChuckinProtoHud::StaticClass();
}
