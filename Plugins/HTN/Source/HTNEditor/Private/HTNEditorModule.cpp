// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditorModule.h"

#include "AssetToolsModule.h"
#include "HTNAssetTypeActions.h"
#include "HTNEditor.h"

void FHTNEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolbarExtensibilityManager = MakeShared<FExtensibilityManager>();

	RegisterAssetActions();
}

void FHTNEditorModule::ShutdownModule()
{
	MenuExtensibilityManager.Reset();
	ToolbarExtensibilityManager.Reset();

	UnregisterAssetActions();
}

void FHTNEditorModule::RegisterAssetActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	TSharedRef<IAssetTypeActions> HTNActions = MakeShared<FHTNAssetTypeActions>();
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

TSharedRef<FHTNEditor> FHTNEditorModule::CreateEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host,
	UHTN* Object)
{
	TSharedRef<FHTNEditor> Editor = MakeShared<FHTNEditor>();
	Editor->InitEditor(Mode, Host, Object);
	return Editor;
}

TSharedPtr<FExtensibilityManager> FHTNEditorModule::GetMenuExtensibilityManager()
{
	return MenuExtensibilityManager;
}

TSharedPtr<FExtensibilityManager> FHTNEditorModule::GetToolbarExtensibilityManager()
{
	return ToolbarExtensibilityManager;
}

IMPLEMENT_MODULE(FHTNEditorModule, HTNEditor)