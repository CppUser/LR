// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraphNode.h"
#include "HTNGraphNode.generated.h"

class UHTNGraphNode;
class UHTNStandaloneNode;

/**
 * 
 */
UCLASS()
class HTNEDITOR_API UHTNGraphNode : public UAIGraphNode
{
	GENERATED_BODY()
public:
	
};


UCLASS()
class HTNEDITOR_API UHTNRootGraphNode : public UHTNGraphNode
{
	GENERATED_BODY()
public:
	UHTNRootGraphNode();
};