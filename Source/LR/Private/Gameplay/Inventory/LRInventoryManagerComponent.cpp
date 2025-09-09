// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Inventory/LRInventoryManagerComponent.h"

#include "Gameplay/Inventory/LRInventoryItemInstance.h"
#include "Gameplay/Inventory/InventoryFragment/LRInventoryItemFragment.h"

/////////////////////////////////////////////////////////////
///                  FInventoryEntry
/////////////////////////////////////////////////////////////
FString FInventoryEntry::GetDebugString() const
{
	TSubclassOf<ULRInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}
/////////////////////////////////////////////////////////////
///             ULRInventoryItemInstance
/////////////////////////////////////////////////////////////
ULRInventoryItemInstance* FInventoryList::AddEntry(TSubclassOf<ULRInventoryItemDefinition> ItemClass, int32 StackCount)
{
	ULRInventoryItemInstance* Result = nullptr;

	check(ItemClass != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<ULRInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemClass);
	for (ULRInventoryItemFragment* Fragment : GetDefault<ULRInventoryItemDefinition>(ItemClass)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	return Result;
}

void FInventoryList::AddEntry(ULRInventoryItemInstance* Instance)
{
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
	FInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	//TODO:UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	//TODO:MessageSystem.BroadcastMessage(TAG_Inventory_Message_StackChanged, Message);
}

/////////////////////////////////////////////////////////////
///            ULRInventoryManagerComponent
/////////////////////////////////////////////////////////////
ULRInventoryManagerComponent::ULRInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, InventoryList(this)
{
}

bool ULRInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<ULRInventoryItemDefinition> ItemDef,
	int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

ULRInventoryItemInstance* ULRInventoryManagerComponent::AddItemDefinition(
	TSubclassOf<ULRInventoryItemDefinition> ItemDef, int32 StackCount)
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
	TSubclassOf<ULRInventoryItemDefinition> ItemDef) const
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

int32 ULRInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<ULRInventoryItemDefinition> ItemDef) const
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

bool ULRInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<ULRInventoryItemDefinition> ItemDef,
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
		if (ULRInventoryItemInstance* Instance = ULRInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
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