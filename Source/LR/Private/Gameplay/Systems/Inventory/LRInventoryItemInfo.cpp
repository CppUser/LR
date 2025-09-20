// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Inventory/LRInventoryItemInfo.h"

#include "Gameplay/Systems/Inventory/LRInventoryItemFragment.h"

const ULRInventoryItemFragment* ULRInventoryItemInfo::FindFragmentByClass(
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
