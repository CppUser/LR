// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNEdGraphSchema.h"

#include "HTNGraphNode.h"

#define LOCTEXT_NAMESPACE "HTNEdSchema"

TMap<uint64, FPinConnectionResponse> UHTNEdGraphSchema::ConnectionCache;

void UHTNEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	Super::CreateDefaultNodesForGraph(Graph);
}

const FPinConnectionResponse UHTNEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (!A || !B)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, FText::GetEmpty());
    
	uint64 CacheKey = (uint64(GetTypeHash(A)) << 32) | GetTypeHash(B);
	if (FPinConnectionResponse* Cached = ConnectionCache.Find(CacheKey))
	{
		return *Cached;
	}
    
	FPinConnectionResponse Response = CanConnect_Fast(A, B);
    
	if (ConnectionCache.Num() > 1024)
	{
		ConnectionCache.Reset();
	}
	ConnectionCache.Add(CacheKey, Response);
    
	return Response;
}

const FPinConnectionResponse UHTNEdGraphSchema::CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const
{
	return Super::CanMergeNodes(A, B);
}

void UHTNEdGraphSchema::GetAssetsGraphHoverMessage(const TArray<FAssetData>& Assets, const UEdGraph* HoverGraph,
	FString& OutTooltipText, bool& OutOkIcon) const
{
	Super::GetAssetsGraphHoverMessage(Assets, HoverGraph, OutTooltipText, OutOkIcon);
}

void UHTNEdGraphSchema::DroppedAssetsOnGraph(const TArray<struct FAssetData>& Assets, const FVector2D& GraphPosition,
	UEdGraph* Graph) const
{
	Super::DroppedAssetsOnGraph(Assets, GraphPosition, Graph);
}

bool UHTNEdGraphSchema::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return Super::IsCacheVisualizationOutOfDate(InVisualizationCacheID);
}

int32 UHTNEdGraphSchema::GetCurrentVisualizationCacheID() const
{
	return Super::GetCurrentVisualizationCacheID();
}

void UHTNEdGraphSchema::ForceVisualizationCacheClear() const
{
	Super::ForceVisualizationCacheClear();
}

void UHTNEdGraphSchema::GetSubNodeClasses(int32 SubNodeFlags, TArray<FGraphNodeClassData>& ClassData,
	UClass*& GraphNodeClass) const
{
	Super::GetSubNodeClasses(SubNodeFlags, ClassData, GraphNodeClass);
}

bool UHTNEdGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	const FPinConnectionResponse Response = CanCreateConnection(A, B);
	if (Response.Response == CONNECT_RESPONSE_MAKE)
	{
		const FScopedTransaction Transaction(LOCTEXT("ConnectPins", "Connect Pins"));
        
		A->Modify();
		B->Modify();
        
		A->MakeLinkTo(B);
        
		//TODO: Mark nodes as dirty for update
		// if (UHTNGraphNode* NodeA = Cast<UHTNGraphNode>(A->GetOwningNode()))
		// {
		// 	NodeA->State.bIsDirty = true;
		// }
		// if (UHTNGraphNode* NodeB = Cast<UHTNGraphNode>(B->GetOwningNode()))
		// {
		// 	NodeB->State.bIsDirty = true;
		// }
        
		return true;
	}
    
	return false;
}

void UHTNEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
}

FConnectionDrawingPolicy* UHTNEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return Super::CreateConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect,
	                                            InDrawElements, InGraphObj);
}

FPinConnectionResponse UHTNEdGraphSchema::CanConnect_Fast(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, 
			LOCTEXT("SameNode", "Cannot connect to same node"));
	}

	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
			A->Direction == EGPD_Input ? 
			LOCTEXT("InputToInput", "Cannot connect input to input") :
			LOCTEXT("OutputToOutput", "Cannot connect output to output"));
	}

	const UHTNGraphNode* NodeA = Cast<UHTNGraphNode>(A->GetOwningNode());
	const UHTNGraphNode* NodeB = Cast<UHTNGraphNode>(B->GetOwningNode());

	if (!NodeA || !NodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
			LOCTEXT("InvalidNode", "Invalid node type"));
	}

	if (NodeA->IsSubNode() || NodeB->IsSubNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
			LOCTEXT("SubNodeConnection", "Sub-nodes cannot have connections"));
	}
    
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, FText::GetEmpty());
}
#undef LOCTEXT_NAMESPACE