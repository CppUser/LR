// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNGraph.h"

#include "HTN.h"
#include "HTNGraphNode.h"

void UHTNGraph::OnCreated()
{
	Super::OnCreated();
	// Initialize the graph
}

void UHTNGraph::UpdateAsset(int32 UpdateFlags)
{
	// Sync visual graph to runtime data
	Super::UpdateAsset(UpdateFlags);

	UHTN* HTNAsset = Cast<UHTN>(GetOuter());
	if (!HTNAsset) return;

    
	// Find root node
	UHTNRootGraphNode* RootNode = FindRootNode();
	if (!RootNode) return;

	// Traverse graph and build runtime representation
	// ... (traverse graph and populate HTNAsset->Nodes)
}

class UHTNRootGraphNode* UHTNGraph::FindRootNode() const
{
	for (UEdGraphNode* Node : Nodes)
	{
		if (UHTNRootGraphNode* RootNode = Cast<UHTNRootGraphNode>(Node))
		{
			return RootNode;
		}
	}
	return nullptr;
}
