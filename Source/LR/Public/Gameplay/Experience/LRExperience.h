// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LRExperience.generated.h"

class ULRPawnData;
/**
 * 
 */
UCLASS(BlueprintType, Const)
class LR_API ULRExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const ULRPawnData> PawnData;
};
