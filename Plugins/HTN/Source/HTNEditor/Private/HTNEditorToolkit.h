// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IHTNEditor.h"
#include "Misc/NotifyHook.h"
#include "AIGraphEditor.h"

/**
 * 
 */

class HTNEDITOR_API HTNEditorToolkit : public IHTNEditor, public FAIGraphEditor, public FNotifyHook
{
public:
	HTNEditorToolkit();
	virtual ~HTNEditorToolkit();
	
	void InitEditorToolkit(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, class UHTN* HTN);

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override;
	virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr) override;
	virtual void FindInContentBrowser_Execute() override;

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
private:
	
	class UHTN* HTNAsset;
};
