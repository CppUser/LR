// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Inventory/LRInventoryItemDefinition.h"

#include "Gameplay/Inventory/InventoryFragment/LRInventoryItemFragment.h"

ULRInventoryItemDefinition::ULRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const ULRInventoryItemFragment* ULRInventoryItemDefinition::FindFragmentByClass(
	TSubclassOf<ULRInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (ULRInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

const ULRInventoryItemFragment* ULRInventoryFunctionLibrary::FindItemDefinitionFragment(
	TSubclassOf<ULRInventoryItemDefinition> ItemDef, TSubclassOf<ULRInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<ULRInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
