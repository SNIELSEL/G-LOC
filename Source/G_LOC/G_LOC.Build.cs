// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class G_LOC : ModuleRules
{
	public G_LOC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore", "UMG", "Slate", "SlateCore"});
	}
}
