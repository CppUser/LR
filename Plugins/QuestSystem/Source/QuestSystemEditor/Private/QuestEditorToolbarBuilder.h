// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class FQuestAssetEditor;

class FQuestEditorToolbarBuilder : public TSharedFromThis<FQuestEditorToolbarBuilder>
{
public:
	FQuestEditorToolbarBuilder(TWeakPtr<FQuestAssetEditor> InQuestEditor) : QuestEditorWeakPtr(InQuestEditor) {}

	void AddModesToolbar(TSharedPtr<class FExtender> Extender);
	void AddAssetToolbar(TSharedPtr<FExtender> Extender);
	void AddDebuggerToolbar(TSharedPtr<class FExtender> Extender);
	
private:
	void FillAssetToolbar(class FToolBarBuilder& ToolbarBuilder);
	void FillModesToolbar(class FToolBarBuilder& ToolbarBuilder);
	void FillDebuggerToolbar(class FToolBarBuilder& ToolbarBuilder);
private:
	TWeakPtr<FQuestAssetEditor> QuestEditorWeakPtr; 
	
};
