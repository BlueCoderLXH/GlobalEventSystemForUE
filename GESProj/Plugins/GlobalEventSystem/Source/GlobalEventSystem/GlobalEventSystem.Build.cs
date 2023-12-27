// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GlobalEventSystem : ModuleRules
{
	public GlobalEventSystem(ReadOnlyTargetRules Target) : base(Target)
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