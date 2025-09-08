// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEdGraphSchema.h"

#include "HTNGraphNode.h"

#define LOCTEXT_NAMESPACE "HTNEditorGraphSchema"

const FPinConnectionResponse UHTNEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Can't connect to self
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, 
			TEXT("Can't connect to self"));
	}
    
	// Can't connect input to input
	if (A->Direction == EGPD_Input && B->Direction == EGPD_Input)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, 
			TEXT("Can't connect input to input"));
	}
    
	// Can't connect output to output
	if (A->Direction == EGPD_Output && B->Direction == EGPD_Output)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, 
			TEXT("Can't connect output to output"));
	}
    
	// Connection is valid!
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UHTNEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create a root node when graph is created
	UHTNRootGraphNode* RootNode = NewObject<UHTNRootGraphNode>(&Graph);
	RootNode->CreateNewGuid();
	RootNode->NodePosX = 0;
	RootNode->NodePosY = 0;
	Graph.AddNode(RootNode);
}

void UHTNEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add menu items for creating nodes
	// This is what appears when you right-click in the graph
    
	// TSharedPtr<FNewNodeAction> NewNodeAction(
	// 	new FNewNodeAction(
	// 		"HTN",
	// 		LOCTEXT("CreateTask", "Create Task"),
	// 		LOCTEXT("CreateTaskTooltip", "Create a new task node"),
	// 		0));
 //            
	// ContextMenuBuilder.AddAction(NewNodeAction);
}
#undef LOCTEXT_NAMESPACE