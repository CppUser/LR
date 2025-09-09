// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditor.h"
#include "HTN.h"
#include "HTNEditorAppModes.h"
#include "HTNTabsFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#define LOCTEXT_NAMESPACE "HTNEditor"

const FName FHTNEditor::HTNMode(TEXT("HTNMode"));

FHTNEditor::FHTNEditor()
	:HTNAsset(nullptr)
	,BlackboardDataAsset(nullptr)
{
}

FHTNEditor::~FHTNEditor()
{
}

void FHTNEditor::InitHTNEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host, class UHTN* HTN)
{
	HTNAsset = HTN;
	if (HTNAsset)
	{
		BlackboardDataAsset = HTNAsset->BlackboardAsset;
	}

	if (!DocumentTracker.IsValid())
	{
		DocumentTracker = MakeShared<FDocumentTracker>();
		DocumentTracker->Initialize(SharedThis(this));
		DocumentTracker->RegisterDocumentFactory(MakeShared<FHTNGraphEditorSummoner>(
			SharedThis(this),
			FOnCreateGraphEditor::CreateSP(this, &FHTNEditor::CreateGraphEditor)
		));
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

		AddApplicationMode(HTNMode, MakeShared<FHTNEditorApplicationMode>(SharedThis(this)));
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

	if (HTNAsset)
	{
		SetCurrentMode(HTNMode);
	}
	else if (BlackboardDataAsset)
	{
		//SetCurrentMode(BlackboardMode);
	}

	RegenerateMenusAndToolbars();
}

UHTN* FHTNEditor::GetHTNAsset() const
{
	return HTNAsset; 
}

void FHTNEditor::SetHTNAsset(UHTN* InHTN)
{
	HTNAsset = InHTN;
}

class UBlackboardData* FHTNEditor::GetCurrentBlackboardData() const
{
	return HTNAsset ? HTNAsset->BlackboardAsset : BlackboardDataAsset;
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

void FHTNEditor::RegisterToolbarTabSpawner(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

FText FHTNEditor::GetLocalizedModeDescription(FName InMode)
{
	static TMap<FName, FText> LocModes;
	if (LocModes.Num() == 0)
	{
		LocModes.Add(HTNMode, LOCTEXT("HTNMode", "Hierarchical Task Network"));
		//TODO:LocModes.Add(BlackboardMode, LOCTEXT("BlackboardMode", "Blackboard"));
	}

	check(InMode != NAME_None);
	const FText* const Description = LocModes.Find(InMode);
	check(Description);
	return *Description;
}

void FHTNEditor::Restore()
{
}

void FHTNEditor::SaveEditedObjectState()
{
}

bool FHTNEditor::IsInEditingMode(bool bGraphIsEditable) const
{
	//TODO: Implement me
	return true;
}

void FHTNEditor::FocusWindow(UObject* ObjectToFocusOn)
{
	FWorkflowCentricApplication::FocusWindow(ObjectToFocusOn);
}

void FHTNEditor::FindInContentBrowser_Execute()
{
	FWorkflowCentricApplication::FindInContentBrowser_Execute();
}

void FHTNEditor::PostUndo(bool bSuccess)
{
	FAIGraphEditor::PostUndo(bSuccess);
}

void FHTNEditor::PostRedo(bool bSuccess)
{
	FAIGraphEditor::PostRedo(bSuccess);
}

void FHTNEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}
FGraphAppearanceInfo FHTNEditor::GetGraphAppearance() const
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "HIERARCHICAL TASK NETWORK");
	return AppearanceInfo;
}
TSharedRef<class SGraphEditor> FHTNEditor::CreateGraphEditor(UEdGraph* InGraph)
{
	SGraphEditor::FGraphEditorEvents GraphEditorEvents;
	GraphEditorEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FHTNEditor::OnSelectedNodesChanged);
	
	const TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("HTNGraphLabel", "Hierarchical Task Network"))
				.TextStyle(FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
			]
		];

	const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(this, &FHTNEditor::IsInEditingMode, bGraphIsEditable)
		.Appearance(this, &FHTNEditor::GetGraphAppearance)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(GraphEditorEvents);
}



#undef LOCTEXT_NAMESPACE
