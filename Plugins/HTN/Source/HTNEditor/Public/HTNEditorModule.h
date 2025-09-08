// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"


class IAssetTypeActions;
class FHTNEditor;

class IHTNEditorModule : public IModuleInterface
{
public:
	static IHTNEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IHTNEditorModule>("HTNEditor");
	}
    
	virtual TSharedRef<FHTNEditor> CreateEditor(
		EToolkitMode::Type Mode,
		TSharedPtr<IToolkitHost> Host,
		class UHTN* HTN) = 0;
        
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
		class UHTN* HTN) override;
        
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override 
	{ 
		return MenuExtensibilityManager; 
	}
    
	virtual TSharedPtr<FExtensibilityManager> GetToolbarExtensibilityManager() override 
	{ 
		return ToolbarExtensibilityManager; 
	}
    
private:
	void RegisterAssetActions();
	void UnregisterAssetActions();
    
	// Cached managers
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolbarExtensibilityManager;

	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetActions;
};