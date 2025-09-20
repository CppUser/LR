// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LRInventoryItemInfo.generated.h"

class ULRInventoryItemFragment;





UCLASS(Blueprintable, Const, Abstract)
class LR_API ULRInventoryItemInfo : public UObject
{
	GENERATED_BODY()
public:
	const ULRInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULRInventoryItemFragment> FragmentClass) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<ULRInventoryItemFragment>> Fragments;

};
