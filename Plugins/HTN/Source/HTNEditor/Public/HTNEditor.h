// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"


class HTNEDITOR_API FHTNEditor : public FWorkflowCentricApplication
{
public:
	FHTNEditor();
	virtual ~FHTNEditor();
    
	// Initialization
	void InitHTNEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host,class UHTN* HTN);

	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
public:
	class UHTN* HTNAsset;
};
