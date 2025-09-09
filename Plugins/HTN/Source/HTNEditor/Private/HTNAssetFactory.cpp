// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNAssetFactory.h"

#include "HTN.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BlackboardData.h"

UHTNAssetFactory::UHTNAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UHTN::StaticClass();
}

UObject* UHTNAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UHTN* NewHTN = NewObject<UHTN>(InParent, Class, Name, Flags);
    
	if (!NewHTN->BlackboardAsset)
	{
		FAssetRegistryModule& AssetRegistry = 
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        
		TArray<FAssetData> BlackboardAssets;
		FTopLevelAssetPath ClassPath = UBlackboardData::StaticClass()->GetClassPathName();
		AssetRegistry.Get().GetAssetsByClass(ClassPath, BlackboardAssets);
        
		if (BlackboardAssets.Num() > 0)
		{
			NewHTN->BlackboardAsset = Cast<UBlackboardData>(
				BlackboardAssets[0].GetAsset());
		}
	}
    
	return NewHTN;
}
