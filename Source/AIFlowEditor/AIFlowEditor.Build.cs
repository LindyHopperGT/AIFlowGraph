// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

using UnrealBuildTool;

public class AIFlowEditor : ModuleRules
{
	public AIFlowEditor(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"AIFlow",
			"Flow",
			"FlowEditor",
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AIModule",
			"Core",
			"CoreUObject",
			"DetailCustomizations",
			"UnrealEd"
		});
	}
}