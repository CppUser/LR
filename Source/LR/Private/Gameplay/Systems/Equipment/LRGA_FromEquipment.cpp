// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Equipment/LRGA_FromEquipment.h"

#include "Gameplay/Systems/Equipment/LREquipmentInstance.h"
#include "Gameplay/Systems/Inventory/LRInventoryItemInstance.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


ULRGA_FromEquipment::ULRGA_FromEquipment(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

ULREquipmentInstance* ULRGA_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<ULREquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

ULRInventoryItemInstance* ULRGA_FromEquipment::GetAssociatedItem() const
{
	if (ULREquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<ULRInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult ULRGA_FromEquipment::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
			{
				Context.AddError(NSLOCTEXT("Lyra", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
				Result = EDataValidationResult::Invalid;
			}

	return Result;
}
#endif