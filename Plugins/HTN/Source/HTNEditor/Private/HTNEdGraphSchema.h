// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraphSchema.h"
#include "HTNEdGraphSchema.generated.h"

/**
 *  schema defines the rules of your graph - what can connect to what.
 */
UCLASS(MinimalAPI)
class UHTNEdGraphSchema : public UAIGraphSchema
{
	GENERATED_BODY()
public:

	// Can we connect these two pins?
	virtual const FPinConnectionResponse CanCreateConnection(
		const UEdGraphPin* A, const UEdGraphPin* B) const override;

	// Create default nodes when graph is created
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	// Get context menu actions
	virtual void GetGraphContextActions(
		FGraphContextMenuBuilder& ContextMenuBuilder) const override;
};
