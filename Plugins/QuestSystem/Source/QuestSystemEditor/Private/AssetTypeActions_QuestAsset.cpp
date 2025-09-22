// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_QuestAsset.h"

#include "Quest.h"
#include "QuestSystemEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_QuestAsset"

FAssetTypeActions_QuestAsset::FAssetTypeActions_QuestAsset(uint32 InCategory)
	:Category(InCategory)
{
}

FText FAssetTypeActions_QuestAsset::GetName() const
{
	return LOCTEXT("Quest", "Dynamic Quest System");
}

FColor FAssetTypeActions_QuestAsset::GetTypeColor() const
{
	return FColor(255, 55, 120);
}

UClass* FAssetTypeActions_QuestAsset::GetSupportedClass() const
{
	return UQuest::StaticClass();
}

void FAssetTypeActions_QuestAsset::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UQuest* Obj = Cast<UQuest>(*ObjIt))
		{
			//TODO:bFoundExisting Instance 
			// bool bFoundExisting = false;
			//
			// FQuestGraphEditor* ExistingInstance = nullptr;
			//
			// if (UAssetEditorSubsystem* AESubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			// {
			// 	ExistingInstance = static_cast<FQuestGraphEditor*>(AESubsystem->FindEditorForAsset(Obj, false));
			// }
			//
			// if (ExistingInstance != nullptr && ExistingInstance->GetQuestAsset() == nullptr)
			// {
			// 	ExistingInstance->InitQuestEditor(Mode, EditWithinLevelEditor, Obj);
			// 	bFoundExisting = true;
			// }
			 
			//if (!bFoundExisting)
			//{
				IQuestSystemEditorModule& QuestEditorModule = IQuestSystemEditorModule::Get();;
				QuestEditorModule.CreateQuestEditor(Mode, EditWithinLevelEditor, Obj);
			//}
		}

	}
}

uint32 FAssetTypeActions_QuestAsset::GetCategories()
{
	return Category;
}
#undef LOCTEXT_NAMESPACE