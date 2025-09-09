// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LRInventoryItemDefinition.generated.h"

class ULRInventoryItemFragment;
/**
 * 
 */
UCLASS(Blueprintable, Const, Abstract)
class LR_API ULRInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	ULRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const ULRInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULRInventoryItemFragment> FragmentClass) const;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<ULRInventoryItemFragment>> Fragments;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class ULRInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const ULRInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<ULRInventoryItemDefinition> ItemDef, TSubclassOf<ULRInventoryItemFragment> FragmentClass);
};
