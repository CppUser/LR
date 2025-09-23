#include "QuestSystemEditor.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_QuestAsset.h"
#include "QuestAssetEditor.h"

#define LOCTEXT_NAMESPACE "FQuestSystemEditorModule"


void FQuestSystemEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
	uint32 QuestAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Quest System")), LOCTEXT("QuestSystemCategory", "QuestSystem"));

	const auto RegisterAssetTypeAction = [&](TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		CreatedAssetTypeActions.Add(Action);
	};
	RegisterAssetTypeAction(MakeShared<FAssetTypeActions_QuestAsset>(QuestAssetCategory));
}

void FQuestSystemEditorModule::ShutdownModule()
{
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();

	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetTools")))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		for (TSharedPtr<IAssetTypeActions>& Action : CreatedAssetTypeActions)
		{
			if (Action.IsValid())
			{
				AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
			}
		}
	}
	CreatedAssetTypeActions.Empty();
}

TSharedRef<class FQuestAssetEditor> FQuestSystemEditorModule::CreateQuestEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, class UQuest* inObj)
{
	TSharedRef<FQuestAssetEditor> NewQuestEditor = MakeShared<FQuestAssetEditor>();
	NewQuestEditor->InitQuestEditor(Mode, InitToolkitHost, inObj);
	return NewQuestEditor;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FQuestSystemEditorModule, QuestSystemEditor)