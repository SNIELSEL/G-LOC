// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Untitled_race_game : ModuleRules
{
	public Untitled_race_game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore", "UMG" });
	}
}
