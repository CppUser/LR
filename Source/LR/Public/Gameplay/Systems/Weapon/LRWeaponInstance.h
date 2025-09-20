// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LRWeaponComboInfo.h"
#include "Gameplay/Systems/Equipment/LREquipmentInstance.h"
#include "LRWeaponInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LR_API ULRWeaponInstance : public ULREquipmentInstance
{
	GENERATED_BODY()
public:
	ULRWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combos")
	bool CanPerformCombo(const FWeaponCombo& Combo) const;

	const FWeaponCombo* GetActiveCombo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combos")
	void SetComboState(const FString& ComboID, int32 ComboIndex);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combos")
	void ResetComboState();

	UFUNCTION(BlueprintPure, Category = "Weapon|Data")
	ULRWeaponComboInfo* GetComboData() const { return ComboData; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Data")
	const FWeaponComboState& GetComboState() const { return ComboState; }

protected:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combos")
	ULRWeaponComboInfo* ComboData;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Weapon|State")
	FWeaponComboState ComboState;

	FTimerHandle UpdateTimerHandle;

};
