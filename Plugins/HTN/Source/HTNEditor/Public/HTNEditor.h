// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "AIGraphEditor.h"

class UHTN;
class UBlackboardData;
class IDetailsView;

class HTNEDITOR_API FHTNEditor : public FWorkflowCentricApplication, public FAIGraphEditor, public FNotifyHook
{
public:
	FHTNEditor();
	virtual ~FHTNEditor();
    
	// Initialization
	void InitHTNEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host,UHTN* HTN);

	virtual UHTN* GetHTNAsset() const;
	virtual void SetHTNAsset(UHTN* HTN);
	virtual class UBlackboardData* GetCurrentBlackboardData() const;
	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	void RegisterToolbarTabSpawner(const TSharedRef<class FTabManager>& InTabManager);
	
	virtual bool IsPrimaryEditor() const override { return true; }
	
	virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr) override;
	virtual void FindInContentBrowser_Execute() override;

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

	static FText GetLocalizedModeDescription(FName InMode);
	void Restore();
	void SaveEditedObjectState();

	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);
private:
	TSharedRef<class SGraphEditor> CreateGraphEditor(UEdGraph* InGraph);
	bool IsInEditingMode(bool bGraphIsEditable) const;
	FGraphAppearanceInfo GetGraphAppearance() const;
public:
	UHTN* HTNAsset;
	UBlackboardData* BlackboardDataAsset;

	TSharedPtr<class FDocumentTracker> DocumentTracker;

	static const FName HTNMode;
};

inline void FHTNEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
}
