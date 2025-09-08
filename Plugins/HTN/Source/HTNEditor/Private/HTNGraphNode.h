// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraphNode.h"
#include "HTNGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class HTNEDITOR_API UHTNGraphNode : public UAIGraphNode
{
	GENERATED_BODY()
public:

	// Create the pins for this node
	virtual void AllocateDefaultPins() override;
    
	// Get the title of this node
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    
	// Can we delete this node?
	virtual bool CanUserDeleteNode() const override { return true; }
    
	// Can we duplicate this node?
	virtual bool CanDuplicateNode() const override { return true; }
};

UCLASS()
class HTNEDITOR_API UHTNRootGraphNode : public UHTNGraphNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "HTN")
	class UBlackboardData* BlackboardAsset;
    
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool CanDuplicateNode() const override { return false; }
};
