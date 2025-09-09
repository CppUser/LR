// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"


class IAssetTypeActions;
class FHTNEditor;
class UHTN;

class IHTNEditorModule : public IModuleInterface
{
public:
	virtual TSharedRef<FHTNEditor> CreateEditor(
		EToolkitMode::Type Mode,
		TSharedPtr<IToolkitHost> Host,
		UHTN* Object) = 0;
	
	static IHTNEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IHTNEditorModule>("HTNEditor");
	}
        
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() = 0;
	virtual TSharedPtr<FExtensibilityManager> GetToolbarExtensibilityManager() = 0;
};


class FHTNEditorModule : public IHTNEditorModule
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedRef<FHTNEditor> CreateEditor(
		EToolkitMode::Type Mode,
		TSharedPtr<IToolkitHost> Host,
		UHTN* Object) override;

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override; 
	virtual TSharedPtr<FExtensibilityManager> GetToolbarExtensibilityManager() override; 
private:
	void RegisterAssetActions();
	void UnregisterAssetActions();
private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolbarExtensibilityManager;

	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetActions;
};
