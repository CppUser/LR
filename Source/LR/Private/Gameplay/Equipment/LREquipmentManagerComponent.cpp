// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Equipment/LREquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Gameplay/Equipment/LREquipmentInfo.h"
#include "Gameplay/Equipment/LREquipmentInstance.h"

ULREquipmentInstance* FEquipmentList::AddEntry(TSubclassOf<ULREquipmentInfo> InEquipmentInfo)
{
	ULREquipmentInstance* Result = nullptr;

	check(InEquipmentInfo != nullptr);
	check(Owner);
	
	const ULREquipmentInfo* EquipmentCDO = GetDefault<ULREquipmentInfo>(InEquipmentInfo);

	TSubclassOf<ULREquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = ULREquipmentInstance::StaticClass();
	}
	
	FEquipmentEntry& NewEntry = Equipment.AddDefaulted_GetRef();
	NewEntry.EquipmentInfo = InEquipmentInfo;
	NewEntry.Instance = NewObject<ULREquipmentInstance>(Owner->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (ULRAbilitySystemComponent* ASC = GetASC())
	{
		for (const TObjectPtr<const ULRAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedAbilities, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	return Result;
}

void FEquipmentList::RemoveEntry(ULREquipmentInstance* InInstance)
{
	for (auto EntryIt = Equipment.CreateIterator(); EntryIt; ++EntryIt)
	{
		FEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == InInstance)
		{
			if (ULRAbilitySystemComponent* ASC = GetASC())
			{
				Entry.GrantedAbilities.TakeFromAbilitySystem(ASC);
			}

			InInstance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

ULRAbilitySystemComponent* FEquipmentList::GetASC() const
{
	check(Owner);
	AActor* OwningActor = Owner->GetOwner();
	return Cast<ULRAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

////////////////////////////////////////////////////////
//         ULREquipmentManagerComponent
////////////////////////////////////////////////////////
ULREquipmentManagerComponent::ULREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, EquipmentList(this)
{
	bWantsInitializeComponent = true;
}

ULREquipmentInstance* ULREquipmentManagerComponent::EquipItem(TSubclassOf<ULREquipmentInfo> EquipmentDefinition)
{
	ULREquipmentInstance* Result = nullptr;
	if (EquipmentDefinition != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result != nullptr)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void ULREquipmentManagerComponent::UnequipItem(ULREquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

void ULREquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void ULREquipmentManagerComponent::UninitializeComponent()
{
	TArray<ULREquipmentInstance*> AllEquipmentInstances;

	for (const FEquipmentEntry& Entry : EquipmentList.Equipment)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (ULREquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}
	
	Super::UninitializeComponent();
}

ULREquipmentInstance* ULREquipmentManagerComponent::GetFirstInstanceOfType(
	TSubclassOf<ULREquipmentInstance> InstanceType)
{
	for (FEquipmentEntry& Entry : EquipmentList.Equipment)
	{
		if (ULREquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<ULREquipmentInstance*> ULREquipmentManagerComponent::GetEquipmentInstancesOfType(
	TSubclassOf<ULREquipmentInstance> InstanceType) const
{
	TArray<ULREquipmentInstance*> Results;
	for (const FEquipmentEntry& Entry : EquipmentList.Equipment)
	{
		if (ULREquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}