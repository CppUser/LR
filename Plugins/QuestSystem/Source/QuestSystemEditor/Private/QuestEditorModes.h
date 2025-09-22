// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"


class FQuestEditorApplicationMode : public FApplicationMode
{
public:
	FQuestEditorApplicationMode(TSharedPtr<class FQuestAssetEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
	virtual void PreDeactivateMode() override;

protected:
	TWeakPtr<class FQuestAssetEditor> EditorWeakPtr;

	FWorkflowAllowedTabSet AllowedTabs;
};

