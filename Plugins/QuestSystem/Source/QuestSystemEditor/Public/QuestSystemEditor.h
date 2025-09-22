#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"

class IAssetTypeActions;

class IQuestSystemEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
    virtual TSharedRef<class FQuestAssetEditor> CreateQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, class UQuest* inObj) = 0;

    static inline IQuestSystemEditorModule& Get()
    {
        return FModuleManager::LoadModuleChecked<IQuestSystemEditorModule>(TEXT("QuestSystemEditor"));
    }
};

class FQuestSystemEditorModule : public IQuestSystemEditorModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual TSharedRef<class FQuestAssetEditor> CreateQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, class UQuest* inObj);
    
    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
    virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

private:
    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
    TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
	
};
