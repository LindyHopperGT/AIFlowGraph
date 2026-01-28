// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

using UnrealBuildTool;
public class AIFlowEditor : ModuleRules
{
	public AIFlowEditor(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Keep these lists in alphabetical order for easier maintenance.
		PublicDependencyModuleNames.AddRange(new[]
		{
			"AIFlow",
			"AssetDefinition",
			"Engine",
			"Flow",
			"FlowDebugger",
			"FlowEditor",
			"InputCore",
			"Slate",
			"SlateCore",
			"ToolMenus",
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AIModule",
			"Core",
			"CoreUObject",
			"DetailCustomizations",
			"EditorStyle",
			"UnrealEd"
		});
	}
}