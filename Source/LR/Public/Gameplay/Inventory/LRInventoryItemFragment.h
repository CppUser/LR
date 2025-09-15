// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
};


UCLASS()
class ULRInventoryFragment_PickupIcon : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FText DisplayName;
};