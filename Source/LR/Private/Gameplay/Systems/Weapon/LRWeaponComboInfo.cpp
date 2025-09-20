// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Weapon/LRWeaponComboInfo.h"

const FWeaponCombo* ULRWeaponComboInfo::FindComboByID(const FString& ComboID) const
{
	if (BasicCombo.ComboID == ComboID)
	{
		return &BasicCombo;
	}
    
	for (const FWeaponCombo& Combo : AdvancedCombos)
	{
		if (Combo.ComboID == ComboID)
		{
			return &Combo;
		}
	}
    
	return nullptr;
}

int32 ULRWeaponComboInfo::GetMaxComboLength() const
{
	int32 MaxLength = BasicCombo.AttackSequence.Num();
    
	for (const FWeaponCombo& Combo : AdvancedCombos)
	{
		MaxLength = FMath::Max(MaxLength, Combo.AttackSequence.Num());
	}
    
	return MaxLength;
}
