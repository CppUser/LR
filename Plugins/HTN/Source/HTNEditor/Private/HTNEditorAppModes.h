// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FHTNEditor;
class FTabManager;

class FHTNEditorApplicationMode : public FApplicationMode
{
public:
	FHTNEditorApplicationMode(TSharedPtr<FHTNEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
protected:
	TWeakPtr<FHTNEditor> Editor;
	FWorkflowAllowedTabSet EditorTabFactories;
};
