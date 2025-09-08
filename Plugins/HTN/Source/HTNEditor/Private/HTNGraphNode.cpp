// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNGraphNode.h"
#define LOCTEXT_NAMESPACE "HTNGraph"
///////////////////////////////////////////////////////////
///                 UHTNGraphNode
///////////////////////////////////////////////////////////
void UHTNGraphNode::AllocateDefaultPins()
{
	// Create input pin
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
    
	// Create output pin
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

FText UHTNGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UnknownNode", "Unknown Node");
}


#undef LOCTEXT_NAMESPACE
///////////////////////////////////////////////////////////
///           UHTNRootGraphNode
///////////////////////////////////////////////////////////
void UHTNRootGraphNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

FText UHTNRootGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return Super::GetNodeTitle(TitleType);
}
///////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////