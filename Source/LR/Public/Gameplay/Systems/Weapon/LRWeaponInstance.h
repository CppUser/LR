// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Systems/Equipment/LREquipmentInstance.h"
#include "LRWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class LR_API ULRWeaponInstance : public ULREquipmentInstance
{
	GENERATED_BODY()
public:
	ULRWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
};
