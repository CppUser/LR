// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditorModule.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_HTN.h"
#include "EdGraphUtilities.h"
#include "HTNEditor.h"
#include "HTNGraphNode.h"
#include "HTNSGraphNode.h"

#define LOCTEXT_NAMESPACE "HTNEditorModule"

namespace
{
	class FGraphPanelNodeFactory_HTN : public FGraphPanelNodeFactory
	{
		virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
		{
			if (UHTNGraphNode* const HTNNode = Cast<UHTNGraphNode>(Node))
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
	ToolbarExtensibilityManager = MakeShared<FExtensibilityManager>();
	
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphNodeFactory = MakeShared<FGraphPanelNodeFactory_HTN>());

	RegisterAssetActions();
}

void FHTNEditorModule::ShutdownModule()
{
	MenuExtensibilityManager.Reset();
	ToolbarExtensibilityManager.Reset();

	if (GraphNodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphNodeFactory);
		GraphNodeFactory.Reset();
	}

	UnregisterAssetActions();
}

TSharedRef<FHTNEditor> FHTNEditorModule::CreateEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host,
	class UHTN* HTN)
{
	TSharedRef<FHTNEditor> Editor = MakeShared<FHTNEditor>();
	Editor->InitHTNEditor(Mode, Host, HTN);
	return Editor;
}

void FHTNEditorModule::RegisterAssetActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	TSharedRef<IAssetTypeActions> HTNActions = MakeShared<AssetTypeActions_HTN>();
	AssetTools.RegisterAssetTypeActions(HTNActions);
	RegisteredAssetActions.Add(HTNActions);
}

void FHTNEditorModule::UnregisterAssetActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (const TSharedPtr<IAssetTypeActions>& Action : RegisteredAssetActions)
		{
			if (Action.IsValid())
			{
				AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
			}
		}
	}
	RegisteredAssetActions.Empty();
}


IMPLEMENT_MODULE(FHTNEditorModule, HTNEditor)

#undef LOCTEXT_NAMESPACE