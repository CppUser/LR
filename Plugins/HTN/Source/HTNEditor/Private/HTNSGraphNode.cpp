// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNSGraphNode.h"

#include "HTNGraphNode.h"

void HTNSGraphNode::UpdateGraphNode()
{
	SGraphNodeAI::UpdateGraphNode();
}

void HTNSGraphNode::CreatePinWidgets()
{
	SGraphNodeAI::CreatePinWidgets();
}

void HTNSGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	SGraphNodeAI::AddPin(PinToAdd);
}

TSharedPtr<SToolTip> HTNSGraphNode::GetComplexTooltip()
{
	return SGraphNodeAI::GetComplexTooltip();
}

void HTNSGraphNode::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize,
	TArray<FOverlayBrushInfo>& Brushes) const
{
	SGraphNodeAI::GetOverlayBrushes(bSelected, WidgetSize, Brushes);
}

TArray<FOverlayWidgetInfo> HTNSGraphNode::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	return SGraphNodeAI::GetOverlayWidgets(bSelected, WidgetSize);
}

TSharedRef<SGraphNode> HTNSGraphNode::GetNodeUnderMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return SGraphNodeAI::GetNodeUnderMouse(MyGeometry, MouseEvent);
}

void HTNSGraphNode::Construct(const FArguments& InArgs, class UHTNGraphNode* InNode)
{
	SGraphNodeAI::Construct({}, InNode);
}
