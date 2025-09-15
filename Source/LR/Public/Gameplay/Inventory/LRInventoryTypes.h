// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"

#include "LRInventoryTypes.generated.h"

class ULRInventoryItemInstance;
enum class EEquipmentSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, ULRInventoryItemInstance*, Item, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EEquipmentSlot, Slot, ULRInventoryItemInstance*, Item);

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None        UMETA(DisplayName = "None"),
	Head        UMETA(DisplayName = "Head"),
	Shoulders   UMETA(DisplayName = "Shoulders"),
	Chest       UMETA(DisplayName = "Chest"),
	Hands       UMETA(DisplayName = "Hands"),
	Legs        UMETA(DisplayName = "Legs"),
	Feet        UMETA(DisplayName = "Feet"),
	MainHand    UMETA(DisplayName = "Main Hand"),
	OffHand     UMETA(DisplayName = "Off Hand"),
	Ring1       UMETA(DisplayName = "Ring 1"),
	Ring2       UMETA(DisplayName = "Ring 2"),
	Trinket1    UMETA(DisplayName = "Trinket 1"),
	Trinket2    UMETA(DisplayName = "Trinket 2"),
	Back        UMETA(DisplayName = "Back"),
	MAX         UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FItemRarity
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText RarityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DropWeight = 100;
};

USTRUCT(BlueprintType)
struct FItemStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, float> BaseStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, float> BonusStats;
};