// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Inventory/LRPickupable.h"

#include "Gameplay/Systems/Inventory/LRInventoryManagerComponent.h"


UPickupableStatics::UPickupableStatics()
: Super(FObjectInitializer::Get())
{
}

TScriptInterface<ILRPickupable> UPickupableStatics::GetFirstPickupableFromActor(AActor* Actor)
{
	TScriptInterface<ILRPickupable> PickupableActor(Actor);
	if (PickupableActor)
	{
		return PickupableActor;
	}

	// If the actor isn't pickupable, it might have a component that has a pickupable interface.
	TArray<UActorComponent*> PickupableComponents = Actor ? Actor->GetComponentsByInterface(ULRPickupable::StaticClass()) : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		// Get first pickupable, if the user needs more sophisticated pickup distinction, will need to be solved elsewhere.
		return TScriptInterface<ILRPickupable>(PickupableComponents[0]);
	}

	return TScriptInterface<ILRPickupable>();
}

void UPickupableStatics::AddPickupToInventory(ULRInventoryManagerComponent* InventoryComponent,
	TScriptInterface<ILRPickupable> Pickup)
{
	if (InventoryComponent && Pickup)
	{
		const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

		for (const FPickupTemplate& Template : PickupInventory.Templates)
		{
			InventoryComponent->AddItemDefinition(Template.ItemDef, Template.StackCount);
		}

		for (const FPickupInstance& Instance : PickupInventory.Instances)
		{
			InventoryComponent->AddItemInstance(Instance.Item);
		}
	}
}
