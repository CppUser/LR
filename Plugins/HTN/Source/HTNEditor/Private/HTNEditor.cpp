#include "HTNEditor.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_HTN.h"
#include "EdGraphUtilities.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FHTNEditorModule"

void FHTNEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
	const auto RegisterAssetTypeAction = [&](TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		CreatedAssetTypeActions.Add(Action);
	};
	RegisterAssetTypeAction(MakeShared<AssetTypeActions_HTN>());
}

void FHTNEditorModule::ShutdownModule()
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

TSharedRef<FHTNEditorModule> FHTNEditorModule::CreateEditor(const EToolkitMode::Type Mode,
                                                            const TSharedPtr<IToolkitHost>& InitToolkitHost, class UHTN* HTN)
{
	TSharedRef<FHTNEditorModule> HTNEditor = MakeShared<FHTNEditorModule>();
	HTNEditor->InitEditor(Mode, InitToolkitHost, HTN);
	return HTNEditor;
}

void FHTNEditorModule::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	class UHTN* HTN)
{
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHTNEditorModule, HTNEditor)