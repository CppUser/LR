// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditorToolkit.h"

#include "GraphEditorActions.h"
#include "HTN.h"

#define LOCTEXT_NAMESPACE "HTNEditor"

HTNEditorToolkit::HTNEditorToolkit() : HTN(nullptr)
{
}

HTNEditorToolkit::~HTNEditorToolkit()
{
}

void HTNEditorToolkit::InitEditorToolkit(const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost, class UHTN* InHTN)
{
	HTNAsset = InHTN;

	TArray<UObject*> ObjectsToEdit;
	if (HTNAsset) ObjectsToEdit.Add(HTNAsset);

	const TArray<UObject*>* const EditedObjects = GetObjectsCurrentlyBeingEdited();
	const bool bIsAlreadyEditingObjects = EditedObjects && EditedObjects->Num();
	if (!bIsAlreadyEditingObjects)
	{
		FGraphEditorCommands::Register();

		FAssetEditorToolkit::InitAssetEditor(
			Mode,
			InitToolkitHost,
			FName(TEXT("HTNApp")), //TODO: Pull it as ID globally ? 
			FTabManager::NewLayout(TEXT("NullLayout"))->AddArea(FTabManager::NewPrimaryArea()),
			true,
			true,
			ObjectsToEdit
		);
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

void HTNEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	IHTNEditor::RegisterTabSpawners(InTabManager);
}

FName HTNEditorToolkit::GetToolkitFName() const
{
	return FName(TEXT("HTNEditor"));
}

FText HTNEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "HTN Editor");
}

FText HTNEditorToolkit::GetToolkitName() const
{
	return IHTNEditor::GetToolkitName();
}

FText HTNEditorToolkit::GetToolkitToolTipText() const
{
	return IHTNEditor::GetToolkitToolTipText();
}

FString HTNEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "HTN ").ToString();
}

FLinearColor HTNEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FColor::Red;
}

bool HTNEditorToolkit::IsPrimaryEditor() const
{ return true; }

void HTNEditorToolkit::FocusWindow(UObject* ObjectToFocusOn)
{
	IHTNEditor::FocusWindow(ObjectToFocusOn);
}

void HTNEditorToolkit::FindInContentBrowser_Execute()
{
	IHTNEditor::FindInContentBrowser_Execute();
}

void HTNEditorToolkit::PostUndo(bool bSuccess)
{
	FAIGraphEditor::PostUndo(bSuccess);
}

void HTNEditorToolkit::PostRedo(bool bSuccess)
{
	FAIGraphEditor::PostRedo(bSuccess);
}

void HTNEditorToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}
#undef LOCTEXT_NAMESPACE