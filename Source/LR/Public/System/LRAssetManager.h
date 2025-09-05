// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "LRAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class LR_API ULRAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static ULRAssetManager& Get();

};
