// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChuckinProto : ModuleRules
{
	public ChuckinProto(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "AIModule", "InputCore", "PhysXVehicles", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "NavigationSystem", "GameplayTasks" });

		PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
