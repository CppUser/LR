// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LRInventoryTypes.h"
#include "UObject/Object.h"
#include "LRInventoryItemFragment.generated.h"


class ULREquipmentInfo;
class ULRInventoryItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class LR_API ULRInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(ULRInventoryItemInstance* Instance) const {}
};


UCLASS()
class ULRInventoryFragment_EquippableItem : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=InventoryFragment)
	TSubclassOf<ULREquipmentInfo> EquipmentDefinition;

	UPROPERTY(EditAnywhere, Category=Equipment)
	EEquipmentSlot RequiredSlot = EEquipmentSlot::None;

	UPROPERTY(EditAnywhere, Category=Equipment)
	TArray<EEquipmentSlot> ValidSlots;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FItemStats ItemStats;
};

UCLASS()
class ULRInventoryFragment_Description : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Description)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Description)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Description)
	FText FlavorText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Description)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Description)
	FItemRarity Rarity;
};


UCLASS()
class ULRInventoryFragment_StackableItem : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stack)
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stack)
	bool bCanSplit = true;
};