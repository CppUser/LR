// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestAssetFactory.h"
#include "Quest.h"

#define LOCTEXT_NAMESPACE "QuestAssetFactory"

UQuestAssetFactory::UQuestAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UQuest::StaticClass();
}

UObject* UQuestAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UQuest>(InParent, Class, Name, Flags);
}

bool UQuestAssetFactory::ConfigureProperties()
{
	//TODO:FClassViewerModule
	return Super::ConfigureProperties();
}

bool UQuestAssetFactory::CanCreateNew() const
{
	return true;
}

FString UQuestAssetFactory::GetDefaultNewAssetName() const
{
	return FString(TEXT("NewQuest"));
}

FText UQuestAssetFactory::GetDisplayName() const
{
	return LOCTEXT("QuestText", "Quest");
}
#undef LOCTEXT_NAMESPACE