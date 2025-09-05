// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/System/LRAssetManager.h"

ULRAssetManager& ULRAssetManager::Get()
{
	check(GEngine);

	if (ULRAssetManager* S = Cast<ULRAssetManager>(GEngine->AssetManager))
	{
		return *S;
	}

	UE_LOG(LogTemp,Fatal,TEXT("Invalid AssetManagerClass in DefaultEngine.ini. It must be set to LRAssetManager"));

	return *NewObject<ULRAssetManager>();
}
