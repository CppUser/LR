#include "HTNEditor.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_HTN.h"
#include "EdGraphUtilities.h"
#include "HTNEditorToolkit.h"
#include "HTNGraphNode.h"
#include "HTNSGraphNode.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FHTNEditorModule"

namespace 
{
	class FGraphPanelNodeFactory_HTN : public FGraphPanelNodeFactory
	{
		virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
		{
			if (UHTNGraphNode* HTNNode = Cast<UHTNGraphNode>(Node))
			{
				return SNew(HTNSGraphNode, HTNNode);
			}
			return nullptr;
		}
	};
	TSharedPtr<FGraphPanelNodeFactory_HTN> GraphNodeFactory;
}


void FHTNEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();

	FEdGraphUtilities::RegisterVisualNodeFactory(GraphNodeFactory = MakeShared<FGraphPanelNodeFactory_HTN>());

	
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();

	HTNAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("HTN")), LOCTEXT("HTNAssetCategory", "HTN"));
	
	const auto RegisterAssetTypeAction = [&](TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		CreatedAssetTypeActions.Add(Action);
	};
	RegisterAssetTypeAction(MakeShared<AssetTypeActions_HTN>(HTNAssetCategoryBit));
}

void FHTNEditorModule::ShutdownModule()
{
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();

	
	if (GraphNodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphNodeFactory);
		GraphNodeFactory.Reset();
	}

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

TSharedRef<IHTNEditor> FHTNEditorModule::CreateEditor(const EToolkitMode::Type Mode,
                                                            const TSharedPtr<IToolkitHost>& InitToolkitHost, class UHTN* HTN)
{
	TSharedRef<HTNEditorToolkit> HTNEditor = MakeShared<HTNEditorToolkit>();
	HTNEditor->InitEditorToolkit(Mode, InitToolkitHost, HTN);
	return HTNEditor;
}



#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHTNEditorModule, HTNEditor)