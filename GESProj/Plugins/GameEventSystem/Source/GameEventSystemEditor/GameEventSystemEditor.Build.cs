using UnrealBuildTool;

public class GameEventSystemEditor : ModuleRules
{
	public GameEventSystemEditor(ReadOnlyTargetRules Target) : base(Target)
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
			"NiagaraCore",
			"Niagara",
			"PropertyEditor",
			"AIModule",
			"AnimationModifiers",
			"ApplicationCore",
			"GameplayTags",
			"WorkspaceMenuStructure",
			"Kismet",
			"LevelSequence",
			"MovieScene",
			"MovieSceneTracks",
			"MovieSceneTools",
			"Json",
			"JsonUtilities",
			"EditorSubsystem",
			"Persona",
			"SkeletonEditor",
			"MaterialAnalyzer",
			"Sequencer"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"BlueprintGraph",
			"KismetCompiler",
			"UnLua",
			"Lua",
			"ToolMenus",
			"ContentBrowser",
			"GameEventSystem",
			"EditorScriptingUtilities",
			"ImageWrapper",
			"MoviePlayer",
			"LevelSequence",
			"Niagara",
			"NiagaraAnimNotifies",
			"Paper2D",
			"GraphEditor"
		});
	}
}