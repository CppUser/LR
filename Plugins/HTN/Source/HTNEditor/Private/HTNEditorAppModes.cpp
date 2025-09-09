// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEditorAppModes.h"

#include "HTNEditor.h"
#include "HTNTabs.h"

FHTNEditorApplicationMode::FHTNEditorApplicationMode(TSharedPtr<FHTNEditor> InEditor)
: FApplicationMode(FHTNEditor::HTNMode, FHTNEditor::GetLocalizedModeDescription),
	Editor(InEditor)
{
	TabLayout = FTabManager::NewLayout(TEXT("Standalone_HTN_Layout_v2"))
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab(FEditorTabIds::GraphEditorID, ETabState::ClosedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.3f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(FEditorTabIds::GraphDetailsID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(FEditorTabIds::BlackboardID, ETabState::OpenedTab)
				)
			)
		);
}

void FHTNEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(Editor.IsValid());
	const TSharedPtr<FHTNEditor> EditorPtr = Editor.Pin();

	EditorPtr->RegisterToolbarTabSpawner(InTabManager.ToSharedRef());
	EditorPtr->PushTabFactories(EditorTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FHTNEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();

	check(Editor.IsValid());
	Editor.Pin()->SaveEditedObjectState();
}

void FHTNEditorApplicationMode::PostActivateMode()
{
	check(Editor.IsValid());
	Editor.Pin()->Restore();
	FApplicationMode::PostActivateMode();
}
