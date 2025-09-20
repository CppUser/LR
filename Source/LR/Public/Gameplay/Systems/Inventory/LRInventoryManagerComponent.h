// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LRInventoryManagerComponent.generated.h"

class ULRInventoryItemInfo;
class ULRInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<ULRInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

USTRUCT(BlueprintType)
struct FInventoryEntry 
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<ULRInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;

	friend struct FInventoryList;
	friend class  ULRInventoryManagerComponent;

};

USTRUCT(BlueprintType)
struct FInventoryList
{
	GENERATED_BODY()

	FInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}
	
	ULRInventoryItemInstance* AddEntry(TSubclassOf<ULRInventoryItemInfo> ItemClass, int32 StackCount);
	void AddEntry(ULRInventoryItemInstance* Instance);
	void RemoveEntry(ULRInventoryItemInstance* Instance);
	
	TArray<ULRInventoryItemInstance*> GetAllItems() const;

private:
	void BroadcastChangeMessage(FInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	friend class ULRInventoryManagerComponent;
};

UCLASS()
class LR_API ULRInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	ULRInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable,  Category=Inventory)
	ULRInventoryItemInstance* AddItemDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddItemInstance(ULRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveItemInstance(ULRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<ULRInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	ULRInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<ULRInventoryItemInfo> ItemDef, int32 NumToConsume);

private:
	UPROPERTY()
	FInventoryList InventoryList;
};
