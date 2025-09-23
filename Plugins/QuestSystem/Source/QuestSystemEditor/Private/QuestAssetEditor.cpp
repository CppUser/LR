// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestAssetEditor.h"
#include "Quest.h"
#include "QuestEditorModes.h"
#include "QuestEditorTabFactories.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"


#define LOCTEXT_NAMESPACE "QuestAssetEditor"

const FName FQuestAssetEditor::ToolkitFName(TEXT("HTNEditor"));
const FName FQuestAssetEditor::QuestMode(TEXT("HTNMode"));

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

	FPropertyEditorModule& PropertyEditorModule = 
		FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bSearchInitialKeyFocus = false;
	DetailsViewArgs.bUpdatesFromSelection = true;
	DetailsViewArgs.bShowOptions = true;
	DetailsViewArgs.bShowModifiedPropertiesOption = false;
    
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(QuestClass);

	

	if (!DocumentTracker.IsValid())
	{
		DocumentTracker = MakeShared<FDocumentTracker>();
		DocumentTracker->Initialize(SharedThis(this));
		DocumentTracker->RegisterDocumentFactory(MakeShared<FQuestGraphEditorSummoner>(
		 	SharedThis(this),
		 	FQuestGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FQuestAssetEditor::CreateGraphEditorWidget)
		 ));
	}
	
	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShared<FQuestEditorToolbarBuilder>(SharedThis(this));
	}

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

		AddApplicationMode(QuestMode, MakeShared<FQuestEditorApplicationMode>(SharedThis(this)));
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

	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(QuestClass);
	}
	
	if (QuestClass)
	{
		SetCurrentMode(QuestMode);
	}
	
	RegenerateMenusAndToolbars();
}

void FQuestAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_QuestEditor", "Quest Editor"));
	DocumentTracker->SetTabManager(InTabManager);
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

FName FQuestAssetEditor::GetToolkitFName() const
{
	return ToolkitFName;
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

FText FQuestAssetEditor::GetLocalizedModeDescription(FName InMode)
{
	static TMap<FName, FText> LocModes;
	if (LocModes.Num() == 0)
	{
		LocModes.Add(QuestMode, LOCTEXT("QuestMode", "Quest System"));
	}

	check(InMode != NAME_None);
	const FText* const Description = LocModes.Find(InMode);
	check(Description);
	return *Description;
}

UQuest* FQuestAssetEditor::GetCurrentQuest() const
{
	return QuestClass;
}

void FQuestAssetEditor::RestoreQuestGraph()
{
	//TODO:
	
}

void FQuestAssetEditor::SaveEditedObjectState()
{
	if (ensure(QuestClass))
	{
		QuestClass->LastEditedDocuments.Reset();
	}

	if (ensure(DocumentTracker.IsValid()))
	{
		DocumentTracker->SaveAllState();
	}
}

void FQuestAssetEditor::RegisterToolbarTabSpawner(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FQuestAssetEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	//TODO: Implement me
}

TSharedRef<class SGraphEditor> FQuestAssetEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{

	if (!InGraph)
	{
		// Return a placeholder if no graph
		return SNew(SGraphEditor)
			.GraphToEdit(nullptr);
	}
	
	const TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush(TEXT("Graph.TitleBackground")))
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
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph);
}

TSharedRef<class SWidget> FQuestAssetEditor::SpawnDetailsWidget()
{
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}

//TODO:
// TSharedRef<class SWidget> FQuestAssetEditor::SpawnSearchWidget()
// {
// 	//TODO:return SAssignNew(SearchView, SQuestSearch, SharedThis(this));
// 	return TSharedRef<class SWidget>();
// }

#undef LOCTEXT_NAMESPACE
