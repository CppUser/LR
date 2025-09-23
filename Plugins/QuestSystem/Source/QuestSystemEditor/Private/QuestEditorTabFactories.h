// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "Widgets/SWidget.h"
#include "GraphEditor.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"


struct FQuestDetailsSummoner : public FWorkflowTabFactory
{
public:
	FQuestDetailsSummoner(TSharedPtr<class FQuestAssetEditor> InQuestEditor);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class FQuestAssetEditor> QuestEditor;
};

struct FQuestSearchSummoner : public FWorkflowTabFactory
{
public:
	FQuestSearchSummoner(TSharedPtr<class FQuestAssetEditor> InQuestEditor);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class FQuestAssetEditor> QuestEditor;
};

class FQuestGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);

	FQuestGraphEditorSummoner(TSharedPtr<class FQuestAssetEditor> InQuestEditor, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);
	
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;
	
protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;
protected:
	TWeakPtr<class FQuestAssetEditor> QuestEditor;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};
