// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GES : ModuleRules
{
	public GES(ReadOnlyTargetRules Target) : base(Target)
	{
		OptimizeCode = CodeOptimization.Never;

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"Engine",
			"CoreUObject",
			"Engine",
			"UnLua",
			"Lua"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnLua",
			"Lua"
		});
	}
}