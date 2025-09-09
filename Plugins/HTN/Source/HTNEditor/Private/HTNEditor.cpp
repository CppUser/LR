// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditor.h"
#include "HTN.h"

#define LOCTEXT_NAMESPACE "HTNEditor"
FHTNEditor::FHTNEditor()
:HTNAsset(nullptr)
,BlackboardDataAsset(nullptr)
{

	
}

FHTNEditor::~FHTNEditor()
{
}

void FHTNEditor::InitEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host, UHTN* HTN)
{
	HTNAsset = HTN;
	if (HTNAsset)
	{
		BlackboardDataAsset = HTNAsset->BlackboardAsset;
	}
	
	TArray<UObject*> ObjectsToEdit;
	if (HTNAsset) ObjectsToEdit.Add(HTNAsset);
	
	const TArray<UObject*>* const EditedObjects = GetObjectsCurrentlyBeingEdited();
	const bool bIsAlreadyEditingObjects = EditedObjects && EditedObjects->Num();
	if (!bIsAlreadyEditingObjects)
	{

		InitAssetEditor(
			Mode,
			Host,
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
	
}

FName FHTNEditor::GetToolkitFName() const
{
	return FName(TEXT("HTNEditor"));
}

FText FHTNEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "HTN Editor");
}

FText FHTNEditor::GetToolkitName() const
{
	return FWorkflowCentricApplication::GetToolkitName();
}

FString FHTNEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "HTN ").ToString();
}

FLinearColor FHTNEditor::GetWorldCentricTabColorScale() const
{
	return FColor::Red;
}

void FHTNEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}
#undef LOCTEXT_NAMESPACE