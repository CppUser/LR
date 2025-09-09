// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Inventory/InventoryFragment/LRInventoryItemFragment.h"

#include "Gameplay/Inventory/LRInventoryItemInstance.h"

void UInventoryFragment_SetStats::OnInstanceCreated(ULRInventoryItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);

	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}
}

int32 UInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
