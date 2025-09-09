// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UBlackboardData;
class UHTN;

class HTNEDITOR_API FHTNEditor : public FWorkflowCentricApplication
{
public:
	FHTNEditor();
	virtual ~FHTNEditor();

	void InitEditor(EToolkitMode::Type Mode, TSharedPtr<IToolkitHost> Host, UHTN* HTN);

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

private:
	UHTN* HTNAsset;
	UBlackboardData* BlackboardDataAsset;
};
