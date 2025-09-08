#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"

class IHTNEditor;
struct FGraphPanelNodeFactory;
struct FGraphNodeClassHelper;
class IAssetTypeActions;

class FHTNEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
    virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual TSharedRef<IHTNEditor> CreateEditor(
        const EToolkitMode::Type Mode,
        const TSharedPtr<IToolkitHost>& InitToolkitHost,
        class UHTN* HTN);


    static FHTNEditorModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FHTNEditorModule>("HTNEditor");
    }
private:
    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
    TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
    EAssetTypeCategories::Type HTNAssetCategoryBit;
};
