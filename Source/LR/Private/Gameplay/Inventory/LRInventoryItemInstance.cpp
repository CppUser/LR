// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Inventory/LRInventoryItemInstance.h"
#include "Gameplay/Inventory/LRInventoryItemFragment.h"

ULRInventoryItemInstance::ULRInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void ULRInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void ULRInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 ULRInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool ULRInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

const ULRInventoryItemFragment* ULRInventoryItemInstance::FindFragmentByClass(
	TSubclassOf<ULRInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<ULRInventoryItemInfo>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

void ULRInventoryItemInstance::SetItemDef(TSubclassOf<ULRInventoryItemInfo> InDef)
{
	ItemDef = InDef;
}
