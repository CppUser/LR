// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestEditorModes.h"

#include "QuestAssetEditor.h"
#include "QuestEditorTabIDs.h"

#define LOCTEXT_NAMESPACE "QuestEditorApplicationMode"

FQuestEditorApplicationMode::FQuestEditorApplicationMode(TSharedPtr<class FQuestAssetEditor> InEditor)
	: FApplicationMode(FQuestAssetEditor::QuestMode,FQuestAssetEditor::GetLocalizedModeDescription)
	, EditorWeakPtr(InEditor)
{
	TabLayout = FTabManager::NewLayout(TEXT("Standalone_HTN_Layout_v2"))
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab(FQuestEditorTabIds::GraphEditorID, ETabState::ClosedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.3f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(FQuestEditorTabIds::GraphDetailsID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(FQuestEditorTabIds::SearchID, ETabState::ClosedTab)
				)
			)
		);

	InEditor->GetToolbarBuilder()->AddAssetToolbar(ToolbarExtender);
	InEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	InEditor->GetToolbarBuilder()->AddDebuggerToolbar(ToolbarExtender);
}

void FQuestEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(EditorWeakPtr.IsValid());
	const TSharedPtr<FQuestAssetEditor> QuestEditorPtr = EditorWeakPtr.Pin();

	QuestEditorPtr->RegisterToolbarTabSpawner(InTabManager.ToSharedRef());
	QuestEditorPtr->PushTabFactories(AllowedTabs);
	
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FQuestEditorApplicationMode::PostActivateMode()
{
	check(EditorWeakPtr.IsValid());
	EditorWeakPtr.Pin()->RestoreQuestGraph();
	
	FApplicationMode::PostActivateMode();
}

void FQuestEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();

	check(EditorWeakPtr.IsValid());
	EditorWeakPtr.Pin()->SaveEditedObjectState();
}
#undef LOCTEXT_NAMESPACE