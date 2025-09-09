// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Equipment/LREquipmentInstance.h"
#include "UObject/Object.h"
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

	UFUNCTION(BlueprintPure)
	float GetTimeSinceLastInteractedWith() const;

protected:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	//TODO: FAnimLayerSelectionSet EquippedAnimSet;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	//TODO: FAnimLayerSelectionSet UneuippedAnimSet;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;
private:
	double TimeLastEquipped = 0.0;

};
