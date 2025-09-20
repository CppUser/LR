// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/LRGA_OpenInventory.h"

#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"
#include "Blueprint/UserWidget.h"
#include "Character/LRCharacter.h"
#include "Gameplay/Systems/Inventory/LRInventoryManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LRPlayerController.h"
#include "UI/Widgets/Inventory/LRInventoryWidget.h"

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

	if (bIsInventoryOpen && bCloseOnInputAgain)
	{
		// Inventory is already open, close it
		CloseInventory();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		// Open the inventory
		OpenInventory();
        
		// Keep the ability active while inventory is open
		// It will end when the inventory is closed
	}
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

void ULRGA_OpenInventory::OpenInventory()
{
	if (bIsInventoryOpen)
	{
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	ALRPlayerController* PC = GetLRPlayerControllerFromActorInfo();
	if (!PC)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer || !InventoryWidgetClass)
	{
		return;
	}

	UCommonActivatableWidget* NewWidget = UCommonUIExtensions::PushContentToLayer_ForPlayer(
		LocalPlayer,
		InventoryLayer,
		InventoryWidgetClass
	);

	if (ULRInventoryWidget* InventoryWidget = Cast<ULRInventoryWidget>(NewWidget))
	{
		CurrentInventoryWidget = InventoryWidget;
        
		if (ALRCharacter* LRCharacter = Cast<ALRCharacter>(AvatarActor))
		{
			if (ULRInventoryManagerComponent* InvComp = LRCharacter->FindComponentByClass<ULRInventoryManagerComponent>())
			{
				InventoryWidget->SetInventoryComponent(InvComp);
			}
		}

		InventoryWidget->OnDeactivated().AddUObject(this, &ThisClass::OnInventoryWidgetDeactivated);
        
		bIsInventoryOpen = true;
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void ULRGA_OpenInventory::CloseInventory()
{
	if (!bIsInventoryOpen)
	{
		return;
	}

	if (CurrentInventoryWidget.IsValid())
	{
		CurrentInventoryWidget->DeactivateWidget();
	}
	else
	{
		bIsInventoryOpen = false;
	}
}

void ULRGA_OpenInventory::OnInventoryWidgetDeactivated()
{
	if (CurrentInventoryWidget.IsValid())
	{
		CurrentInventoryWidget->OnDeactivated().RemoveAll(this);
	}
    
	CurrentInventoryWidget.Reset();
	bIsInventoryOpen = false;
    
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
