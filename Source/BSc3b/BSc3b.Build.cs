// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BSc3b : ModuleRules
{
	public BSc3b(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Niagara", "UMG", "OnlineSubsystemEOS", "OnlineSubsystem", "OnlineSubsystemUtils" });
	}
}
