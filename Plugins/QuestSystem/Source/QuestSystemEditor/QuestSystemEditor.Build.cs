using UnrealBuildTool;

public class QuestSystemEditor : ModuleRules
{
    public QuestSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ApplicationCore",
                "AssetTools",
                "Projects",
                "InputCore",
                "UnrealEd",
                "LevelEditor",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "SourceControl",
                "EditorStyle",
                "GraphEditor",
                "Kismet",
                "KismetWidgets",
                "GameplayTasks",
                "AIModule",
                "ToolMenus",
                "PropertyEditor",
                "QuestSystem"
            }
        );
    }
}