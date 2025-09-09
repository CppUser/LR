// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNTabsFactory.h"

#include "HTN.h"
#include "Styling/AppStyle.h"
#include "HTNTabs.h"

FHTNGraphEditorSummoner::FHTNGraphEditorSummoner(TSharedPtr<class FHTNEditor> InEditor,
                                                 FOnCreateGraphEditor CreateGraphEditorCallback)
:FDocumentTabFactoryForObjects<UEdGraph>(FEditorTabIds::GraphDetailsID, InEditor),
Editor(InEditor),
OnCreateGraphEditor(CreateGraphEditorCallback)
{
}

void FHTNGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(Editor.IsValid());
	const TSharedRef<SGraphEditor> GraphEditorWidget = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	Editor.Pin()->OnGraphEditorFocused(GraphEditorWidget);
}

void FHTNGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FHTNGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FHTNGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info,
	UEdGraph* DocumentID) const
{
	return OnCreateGraphEditor.Execute(DocumentID);
}

const FSlateBrush* FHTNGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info,
	UEdGraph* DocumentID) const
{
	return FAppStyle::GetBrush("NoBrush");
}


void FHTNGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(Editor.IsValid());
	check(Editor.Pin()->GetHTNAsset());

	UEdGraph* const Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	
	FVector2D ViewLocation;
	float ZoomAmount;
	const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
	
	//TODO:Editor.Pin()->GetHTNAsset()->LastEditedDocuments.Emplace(Graph, ViewLocation, ZoomAmount);
}
