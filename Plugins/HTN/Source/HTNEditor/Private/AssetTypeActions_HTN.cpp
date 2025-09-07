#include "AssetTypeActions_HTN.h"

#include "HTN.h"
#include "HTNEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_HTN"

FText AssetTypeActions_HTN::GetName() const
{
	return LOCTEXT("HTN", "Hierarchical Task Network");
}

FColor AssetTypeActions_HTN::GetTypeColor() const
{
	//TODO: Get Custom color ???
	return FColor(255, 0, 0); 
}

UClass* AssetTypeActions_HTN::GetSupportedClass() const
{
	return UHTN::StaticClass();
}

uint32 AssetTypeActions_HTN::GetCategories()
{
	//return EAssetTypeCategories::; 
	return IAssetTools::Get().FindAdvancedAssetCategory(TEXT("AI"));
}

void AssetTypeActions_HTN::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// for (UObject* Object : InObjects)
	// {
	// 	if (UHTN* HTN = Cast<UHTN>(Object))
	// 	{
	// 		FHTNEditorModule& HTNEditorModule = 
	// 			FModuleManager::LoadModuleChecked<FHTNEditorModule>("HTNEditor");
	// 		HTNEditorModule.CreateHTNEditor(
	// 			EditWithinLevelEditor.IsValid() ? 
	// 				EToolkitMode::WorldCentric : 
	// 				EToolkitMode::Standalone,
	// 			EditWithinLevelEditor, 
	// 			HTN);
	// 	}
	// }

	
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	FHTNEditorModule& EditorModule = FHTNEditorModule::Get();
	
	for (UObject* const Object : InObjects)
	{
		if (UHTN* const HTN = Cast<UHTN>(Object))
		{
			EditorModule.CreateEditor(Mode, EditWithinLevelEditor, HTN);
		}
	}
}
#undef LOCTEXT_NAMESPACE