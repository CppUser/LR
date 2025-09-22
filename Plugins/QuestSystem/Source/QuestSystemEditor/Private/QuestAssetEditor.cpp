// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestAssetEditor.h"
#include "Quest.h"

#define LOCTEXT_NAMESPACE "QuestAssetEditor"

FQuestAssetEditor::FQuestAssetEditor()
	:QuestClass(nullptr)
{
}

FQuestAssetEditor::~FQuestAssetEditor()
{
}

void FQuestAssetEditor::InitQuestEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost, class UQuest* inObj)
{
	QuestClass = inObj;

	TArray<UObject*> ObjectsToEdit;
	if (QuestClass) ObjectsToEdit.Add(QuestClass);

	const TArray<UObject*>* const EditedObjects = GetObjectsCurrentlyBeingEdited();
	const bool bIsAlreadyEditingObjects = EditedObjects && EditedObjects->Num();
	if (!bIsAlreadyEditingObjects)
	{
		FAssetEditorToolkit::InitAssetEditor(
			Mode,
			InitToolkitHost,
			FName(TEXT("QuestEditorApp")), //TODO: Temp
			FTabManager::NewLayout(TEXT("NullLayout"))->AddArea(FTabManager::NewPrimaryArea()),
			true,
			true,
			ObjectsToEdit
		);

		//TODO: Add App modes
	}
	else
	{
		for (UObject* const ObjectToEdit : ObjectsToEdit)
		{
			if (!EditedObjects->Contains(ObjectToEdit))
			{
				AddEditingObject(ObjectToEdit);
			}
		}
	}

	
	RegenerateMenusAndToolbars();
}

void FQuestAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

FName FQuestAssetEditor::GetToolkitFName() const
{
	//TODO:return ToolkitFName;
	return FName("Quest Editor");
}

FText FQuestAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "QuestEditor");
}

FText FQuestAssetEditor::GetToolkitName() const
{
	//TODO: FIX 
	// if (const UObject* EditingObject = GetCurrentMode() == QuestMode ? StaticCast<UObject*>(QuestClass) : nullptr)
	// {
	// 	return FAssetEditorToolkit::GetLabelForObject(EditingObject);
	// }
	// OR
	if (const UObject* EditingObject = StaticCast<UObject*>(QuestClass))  
	{
		return FAssetEditorToolkit::GetLabelForObject(EditingObject);
	}
		
	return FText();
}

FText FQuestAssetEditor::GetToolkitToolTipText() const
{
	if (const UObject* EditingObject = StaticCast<UObject*>(QuestClass))  
	{
		return FAssetEditorToolkit::GetLabelForObject(EditingObject);
	}
		
	return FText();
}

FString FQuestAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Quest ").ToString(); 
}

FLinearColor FQuestAssetEditor::GetWorldCentricTabColorScale() const
{
	return FColor::Yellow;
}

void FQuestAssetEditor::FocusWindow(UObject* ObjectToFocusOn)
{
	FWorkflowCentricApplication::FocusWindow(ObjectToFocusOn);
}

void FQuestAssetEditor::FindInContentBrowser_Execute()
{
	TArray<UObject*> Objects;
	Objects.Add(QuestClass);
	//TODO: add other assets ?
	GEditor->SyncBrowserToObjects(Objects);
}

#undef LOCTEXT_NAMESPACE