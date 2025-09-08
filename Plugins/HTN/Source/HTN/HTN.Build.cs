// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HTN : ModuleRules
{
	public HTN(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",     
				"AIModule",          
				"GameplayTasks",     
				"NavigationSystem"   
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"Json"
			}
		);
	}
}
