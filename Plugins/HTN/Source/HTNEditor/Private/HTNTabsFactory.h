// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "Widgets/SWidget.h"
#include "GraphEditor.h"
#include "HTNEditor.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditor, UEdGraph*);
	
class FHTNGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	FHTNGraphEditorSummoner(TSharedPtr<class FHTNEditor> InEditor, FOnCreateGraphEditor CreateGraphEditorCallback);
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;
protected:
	TWeakPtr<class FHTNEditor> Editor;
	FOnCreateGraphEditor OnCreateGraphEditor;
};