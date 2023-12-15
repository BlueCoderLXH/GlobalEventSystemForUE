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
			"InputCore",
			"Slate",
			"SlateCore",
			"UMG",
			"UnrealEd",
			"UMGEditor",
			"ContentBrowser",
			"EditorStyle",
			"PropertyEditor",
			"WorkspaceMenuStructure",
			"Kismet",
			"EditorSubsystem",
			"BlueprintGraph",
			"GlobalEventSystem",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"KismetCompiler",
			"UnLua",
			"ToolMenus",
			"GlobalEventSystem",
			"EditorScriptingUtilities",
			"Paper2D",
			"GraphEditor",
		});
	}
}