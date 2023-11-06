using UnrealBuildTool;

public class GlobalEventSystemEditor : ModuleRules
{
	public GlobalEventSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
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
			"EditorSubsystem"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"BlueprintGraph",
			"KismetCompiler",
			"UnLua",
			"Lua",
			"ToolMenus",
			"ContentBrowser",
			"GlobalEventSystem",
			"EditorScriptingUtilities",
			"Paper2D",
			"GraphEditor"
		});
	}
}