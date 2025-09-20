// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Weapon/LRWeaponInstance.h"

ULRWeaponInstance::ULRWeaponInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ComboState.Reset();
}

bool ULRWeaponInstance::CanPerformCombo(const FWeaponCombo& Combo) const
{
	//TODO: Implement me
	return true;
}

const FWeaponCombo* ULRWeaponInstance::GetActiveCombo() const
{
	if (ComboData || !ComboState.ActiveComboID.IsEmpty())
	{
		return ComboData->FindComboByID(ComboState.ActiveComboID);
	}
    
	return nullptr;
}

void ULRWeaponInstance::SetComboState(const FString& ComboID, int32 ComboIndex)
{
	ComboState.ActiveComboID = ComboID;
	ComboState.CurrentComboIndex = ComboIndex;
	ComboState.LastAttackTime = GetWorld()->GetTimeSeconds();
}

void ULRWeaponInstance::ResetComboState()
{
	ComboState.CurrentComboIndex = 0;
	ComboState.ActiveComboID = "";
	ComboState.bComboWindowOpen = false;
}

void ULRWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	ComboState.Reset();
}

void ULRWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}
