// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "SGraphNodeAI.h"


class HTNSGraphNode : public SGraphNodeAI
{
public:
	SLATE_BEGIN_ARGS(HTNSGraphNode) {}
	SLATE_END_ARGS()
    
	void Construct(const FArguments& InArgs, class UHTNGraphNode* InNode);
    
	// Update the visual representation
	virtual void UpdateGraphNode() override;
    
	// Create the widget for the node's content
	virtual TSharedRef<SWidget> CreateNodeContentArea();

	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
};
