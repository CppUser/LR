// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/LRGA_OpenInventory.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ULRGA_OpenInventory::ULRGA_OpenInventory(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	ActivationGroup = EAbilityActivationGroup::Independent;
}

void ULRGA_OpenInventory::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ToggleInventory();
}

void ULRGA_OpenInventory::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bIsInventoryOpen)
	{
		CloseInventory();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULRGA_OpenInventory::ToggleInventory()
{
	if (bIsInventoryOpen)
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
	}
}

void ULRGA_OpenInventory::OpenInventory()
{
	
}

void ULRGA_OpenInventory::CloseInventory()
{
	
}
