// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNSGraphNode.h"

#include "EditorStyleSet.h"
#include "HTNGraphNode.h"

#define LOCTEXT_NAMESPACE "HTNEditor"

void HTNSGraphNode::Construct(const FArguments& InArgs, class UHTNGraphNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
}

void HTNSGraphNode::UpdateGraphNode()
{
	// Clear old widgets
    InputPins.Empty();
    OutputPins.Empty();
    
    // Reset the widget
    RightNodeBox.Reset();
    LeftNodeBox.Reset();
    
    // Create the main node content
    TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
    
    // Build the node's visual structure
    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
            .Padding(0)
            .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f))
            [
                SNew(SHorizontalBox)
                
                // Left side (input pins)
                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(LeftNodeBox, SVerticalBox)
                ]
                
                // Center (node content)
                +SHorizontalBox::Slot()
                .Padding(10.0f)
                [
                    SNew(SVerticalBox)
                    
                    // Node title
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
                    ]
                    
                    // Node description
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("NodeDesc", "HTN Node"))
                    ]
                ]
                
                // Right side (output pins)
                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(RightNodeBox, SVerticalBox)
                ]
            ]
        ];

    UHTNGraphNode* HTNNode = Cast<UHTNGraphNode>(GraphNode);
    if (!HTNNode) return;
    
    for (UEdGraphPin* Pin : HTNNode->Pins)
    {
        if (!Pin->bHidden)
        {
            TSharedPtr<SGraphPin> NewPin = SNew(SGraphPin, Pin);
            AddPin(NewPin.ToSharedRef());
        }
    }
    
    
    // Create pins
    CreatePinWidgets();
}

TSharedRef<SWidget> HTNSGraphNode::CreateNodeContentArea()
{
	return SGraphNodeAI::CreateNodeContentArea();
}

TSharedPtr<SGraphPin> HTNSGraphNode::CreatePinWidget(UEdGraphPin* Pin) const
{
    
    return SGraphNodeAI::CreatePinWidget(Pin);
}
#undef LOCTEXT_NAMESPACE
