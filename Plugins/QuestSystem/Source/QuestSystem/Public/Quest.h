// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Quest.generated.h"

/**
 * 
 */
UCLASS()
class QUESTSYSTEM_API UQuest : public UObject
{
	GENERATED_BODY()
public:

#if WITH_EDITORONLY_DATA
	// The editor graph associated with this Quest.
	UPROPERTY()
	class UEdGraph* QuestGraph;

	// Contextual info stored on editor close. Viewport location, zoom level etc.
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif
};
