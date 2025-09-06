#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"

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

    static FHTNEditorModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FHTNEditorModule>("HTNModule");
    }
private:
    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
    TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;


};
