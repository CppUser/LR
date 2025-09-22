// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "QuestEditorToolbarBuilder.h"

class QUESTSYSTEMEDITOR_API FQuestAssetEditor : public FWorkflowCentricApplication
{
public:
	FQuestAssetEditor();
	virtual ~FQuestAssetEditor();
	
	void InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, class UQuest* inObj);

	//~ Begin FAssetEditorToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override { return true; }
	virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr) override;
	virtual void FindInContentBrowser_Execute() override;
	// End FAssetEditorToolkit interface

	static FText GetLocalizedModeDescription(FName InMode);
	
	FORCEINLINE TSharedPtr<class FQuestEditorToolbarBuilder> GetToolbarBuilder() const { return ToolbarBuilder; }

	void RestoreQuestGraph();
	void SaveEditedObjectState();
	void RegisterToolbarTabSpawner(const TSharedRef<class FTabManager>& InTabManager);
	
public:
	static const FName ToolkitFName;
	static const FName QuestMode;
private:
	class UQuest* QuestClass;

	TSharedPtr<class IDetailsView> DetailsView;
	TSharedPtr<class FDocumentTracker> DocumentTracker;
	TSharedPtr<class FQuestEditorToolbarBuilder> ToolbarBuilder;

};
