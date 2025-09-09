// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/LRGameplayAbility.h"
#include "UObject/Object.h"
#include "LRGA_FromEquipment.generated.h"

class ULREquipmentInstance;
class ULRInventoryItemInstance;


UCLASS()
class LR_API ULRGA_FromEquipment : public ULRGameplayAbility
{
	GENERATED_BODY()
public:
	ULRGA_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="LR|Ability")
	ULREquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Ability")
	ULRInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
