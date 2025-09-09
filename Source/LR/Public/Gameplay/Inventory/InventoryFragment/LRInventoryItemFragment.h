// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "LRInventoryItemFragment.generated.h"

class ULREquipmentDefinition;
class ULRInventoryItemInstance;


UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class LR_API ULRInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(ULRInventoryItemInstance* Instance) const {}
};

UCLASS()
class UInventoryFragment_EquippableItem : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Lyra)
	TSubclassOf<ULREquipmentDefinition> EquipmentDefinition;
};

UCLASS()
class UInventoryFragment_PickupIcon : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UInventoryFragment_PickupIcon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FText DisplayName;
};

UCLASS()
class UInventoryFragment_QuickBarIcon : public ULRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush Brush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush AmmoBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FText DisplayNameWhenEquipped;
};

UCLASS()
class UInventoryFragment_SetStats : public ULRInventoryItemFragment
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(ULRInventoryItemInstance* Instance) const override;
	int32 GetItemStatByTag(FGameplayTag Tag) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

};