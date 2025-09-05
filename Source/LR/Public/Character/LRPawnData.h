// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Engine/DataAsset.h"
#include "LRPawnData.generated.h"

class ULRInputConfig;
class ULRAbilitySet;
class APawn;

UCLASS(BlueprintType, Const)
class LR_API ULRPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Abilities")
	TArray<TObjectPtr<ULRAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<ULRInputConfig> InputConfig;
};
