// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Equipment/LREquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Gameplay/Equipment/LREquipmentDefinition.h"
#include "Gameplay/Equipment/LREquipmentInstance.h"

///////////////////////////////////////////////////////////
///                  FAppliedEquipmentEntry
///////////////////////////////////////////////////////////
FString FAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}
///////////////////////////////////////////////////////////
///                    FEquipmentList
///////////////////////////////////////////////////////////
ULREquipmentInstance* FEquipmentList::AddEntry(TSubclassOf<ULREquipmentDefinition> EquipmentDefinition)
{
	ULREquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	
	const ULREquipmentDefinition* EquipmentCDO = GetDefault<ULREquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<ULREquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = ULREquipmentInstance::StaticClass();
	}
	
	FAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<ULREquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (ULRAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (const TObjectPtr<const ULRAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	return Result;
}

void FEquipmentList::RemoveEntry(ULREquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (ULRAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

ULRAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<ULRAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}
///////////////////////////////////////////////////////////
///             ULREquipmentManagerComponent
///////////////////////////////////////////////////////////
ULREquipmentManagerComponent::ULREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, EquipmentList(this)
{
	bWantsInitializeComponent = true;
}

ULREquipmentInstance* ULREquipmentManagerComponent::EquipItem(TSubclassOf<ULREquipmentDefinition> EquipmentDefinition)
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

	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
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
	for (FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
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
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
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