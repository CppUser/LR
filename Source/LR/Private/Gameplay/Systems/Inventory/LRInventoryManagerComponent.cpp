// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Inventory/LRInventoryManagerComponent.h"
#include "NativeGameplayTags.h"
#include "Gameplay/Systems/Inventory/LRInventoryItemFragment.h"
#include "Gameplay/Systems/Inventory/LRInventoryItemInstance.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_LR_Inventory_Item_Count, "LR.Inventory.Item.Count");

ULRInventoryItemInstance* FInventoryList::AddEntry(TSubclassOf<ULRInventoryItemInfo> ItemClass, int32 StackCount)
{
	ULRInventoryItemInstance* Result = nullptr;

	check(ItemClass != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<ULRInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemClass);
	for (ULRInventoryItemFragment* Fragment : GetDefault<ULRInventoryItemInfo>(ItemClass)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	NewEntry.Instance->AddStatTagStack(TAG_LR_Inventory_Item_Count, StackCount);
	

	BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
	
	return Result;
}

void FInventoryList::AddEntry(ULRInventoryItemInstance* Instance)
{
	//TODO: Implement
	unimplemented();
}

void FInventoryList::RemoveEntry(ULRInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
		}
	}
}

TArray<ULRInventoryItemInstance*> FInventoryList::GetAllItems() const
{
	TArray<ULRInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

void FInventoryList::BroadcastChangeMessage(FInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	//TODO: implement UGameplayMessageSubsystem
}

////////////////////////////////////////////////////////
///             ULRInventoryManagerComponent
////////////////////////////////////////////////////////
ULRInventoryManagerComponent::ULRInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, InventoryList(this)
{
}

bool ULRInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

ULRInventoryItemInstance* ULRInventoryManagerComponent::AddItemDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef,
	int32 StackCount)
{
	ULRInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
	}
	return Result;
}

void ULRInventoryManagerComponent::AddItemInstance(ULRInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
}

void ULRInventoryManagerComponent::RemoveItemInstance(ULRInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);
}

TArray<ULRInventoryItemInstance*> ULRInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

ULRInventoryItemInstance* ULRInventoryManagerComponent::FindFirstItemStackByDefinition(
	TSubclassOf<ULRInventoryItemInfo> ItemDef) const
{
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		ULRInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 ULRInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		ULRInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool ULRInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef,
	int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (ULRInventoryItemInstance* Instance = FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}
