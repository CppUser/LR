// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraph.h"
#include "HTNGraph.generated.h"

/**
 * 
 */
UCLASS()
class HTNEDITOR_API UHTNGraph : public UAIGraph
{
	GENERATED_BODY()
public:
	// Called when graph is created
	virtual void OnCreated();

	// Update the runtime asset from the visual graph
	virtual void UpdateAsset();

	// Find the root node
	class UHTNRootGraphNode* FindRootNode() const;
};

