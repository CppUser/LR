// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestEditorTabFactories.h"

#include "Quest.h"
#include "QuestAssetEditor.h"
#include "QuestEditorTabIDs.h"

#define LOCTEXT_NAMESPACE "QuestEditorFactories"
///////////////////////////////////////////////////////////////////////
///                FQuestDetailsSummoner
//////////////////////////////////////////////////////////////////////
FQuestDetailsSummoner::FQuestDetailsSummoner(TSharedPtr<FQuestAssetEditor> InQuestEditor)
: FWorkflowTabFactory(FQuestEditorTabIds::GraphDetailsID, InQuestEditor)
	, QuestEditor(InQuestEditor)
{
	TabLabel = LOCTEXT("QuestDetailsLabel", "Details");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.Tabs.Components");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("QuestDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("QuestDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FQuestDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(QuestEditor.IsValid());
	return QuestEditor.Pin()->SpawnDetailsWidget();
}

FText FQuestDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("QuestDetailsTabTooltip", "The Quest details tab allows editing of the properties of Quest nodes");
}
///////////////////////////////////////////////////////////////////////
///                 FQuestSearchSummoner
//////////////////////////////////////////////////////////////////////
FQuestSearchSummoner::FQuestSearchSummoner(TSharedPtr<class FQuestAssetEditor> InQuestEditor)
: FWorkflowTabFactory(FQuestEditorTabIds::SearchID, InQuestEditor)
	, QuestEditor(InQuestEditor)
{
	TabLabel = LOCTEXT("QuestTreeSearchLabel", "Search");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.Tabs.FindResults");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("QuestSearchView", "Search");
	ViewMenuTooltip = LOCTEXT("QuestSearchView_ToolTip", "Show the Quest search tab");
}

TSharedRef<SWidget> FQuestSearchSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::CreateTabBody(Info);
}
//TODO:
// TSharedRef<SWidget> FQuestSearchSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
// {
// 	check(QuestEditor.IsValid());
// 	return QuestEditor.Pin()->SpawnSearchWidget();
// }

FText FQuestSearchSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("QuestSearchTabTooltip", "The Quest search tab allows searching within Quest nodes");
}

///////////////////////////////////////////////////////////////////////
///                 FQuestGraphEditorSummoner
//////////////////////////////////////////////////////////////////////

FQuestGraphEditorSummoner::FQuestGraphEditorSummoner(TSharedPtr<class FQuestAssetEditor> InQuestEditor,
	FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
:FDocumentTabFactoryForObjects<UEdGraph>(FQuestEditorTabIds::GraphDetailsID, InQuestEditor),
	QuestEditor(InQuestEditor),
	OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{
}

void FQuestGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(QuestEditor.IsValid());
	const TSharedRef<SGraphEditor> GraphEditorWidget = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	QuestEditor.Pin()->OnGraphEditorFocused(GraphEditorWidget);
}

void FQuestGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FQuestGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FQuestGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info,
	UEdGraph* DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FQuestGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info,
	UEdGraph* DocumentID) const
{
	return FAppStyle::Get().GetBrush("GraphEditor.TabIcon");
}

void FQuestGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(QuestEditor.IsValid());
	check(QuestEditor.Pin()->GetCurrentQuest());

	UEdGraph* const Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	
	FVector2D ViewLocation;
	float ZoomAmount;
	const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
	
	QuestEditor.Pin()->GetCurrentQuest()->LastEditedDocuments.Emplace(Graph, ViewLocation, ZoomAmount);
}

#undef LOCTEXT_NAMESPACE