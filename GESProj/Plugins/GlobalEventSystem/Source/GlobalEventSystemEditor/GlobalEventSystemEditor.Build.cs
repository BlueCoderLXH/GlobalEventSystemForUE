using UnrealBuildTool;

public class GlobalEventSystemEditor : ModuleRules
{
	public GlobalEventSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		OptimizeCode = CodeOptimization.Never;
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UnrealEd",
			"Kismet",
			"BlueprintGraph",
			"GlobalEventSystem",
			"KismetCompiler",
			"GlobalEventSystem",
			"UnLua",
			"Lua"
		});
	}
}