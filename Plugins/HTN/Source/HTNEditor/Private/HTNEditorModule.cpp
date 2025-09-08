// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditorModule.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_HTN.h"
#include "HTNEditor.h"

#define LOCTEXT_NAMESPACE "HTNEditorModule"

void FHTNEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolbarExtensibilityManager = MakeShared<FExtensibilityManager>();

	
	RegisterAssetActions();
}

void FHTNEditorModule::ShutdownModule()
{
	UnregisterAssetActions();

	MenuExtensibilityManager.Reset();
	ToolbarExtensibilityManager.Reset();
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