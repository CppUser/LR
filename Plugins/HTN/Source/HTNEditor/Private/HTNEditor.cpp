#include "HTNEditor.h"

#include "AssetToolsModule.h"
#include "EdGraphUtilities.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FHTNEditorModule"




void FHTNEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();

	
}

void FHTNEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHTNEditorModule, HTNEditor)