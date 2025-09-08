// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_HTN.h"

#include "HTN.h"
#include "HTNEditorModule.h"


FText AssetTypeActions_HTN::GetName() const
{
	return FText::FromString("HTN");
}

FColor AssetTypeActions_HTN::GetTypeColor() const
{
	return FColor(255, 0, 0); 
}

UClass* AssetTypeActions_HTN::GetSupportedClass() const
{
	return UHTN::StaticClass();
}

uint32 AssetTypeActions_HTN::GetCategories()
{
	return IAssetTools::Get().FindAdvancedAssetCategory("Artificial Intelligence");
}

void AssetTypeActions_HTN::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* const Object : InObjects)
	{
		if (UHTN* const HTN = Cast<UHTN>(Object))
		{
			FHTNEditorModule::Get().CreateEditor(Mode, EditWithinLevelEditor, HTN);
		}
	}
}
