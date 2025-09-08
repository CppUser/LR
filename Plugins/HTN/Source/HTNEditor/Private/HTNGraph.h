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
	virtual void OnCreated() override;
	virtual void OnLoaded() override;
	virtual void UpdateAsset(int32 UpdateFlags = 0) override;
    
};
