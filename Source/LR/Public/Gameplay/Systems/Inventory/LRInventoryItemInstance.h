// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LRInventoryItemInfo.h"
#include "UObject/Object.h"
#include "Utils/LRGameplayTagStack.h"
#include "LRInventoryItemInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LR_API ULRInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	ULRInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	TSubclassOf<ULRInventoryItemInfo> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const ULRInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULRInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
	void SetItemDef(TSubclassOf<ULRInventoryItemInfo> InDef);
private:
	UPROPERTY()
	FGameplayTagStackContainer StatTags;

	UPROPERTY()
	TSubclassOf<ULRInventoryItemInfo> ItemDef;

	friend struct FInventoryList;
};
