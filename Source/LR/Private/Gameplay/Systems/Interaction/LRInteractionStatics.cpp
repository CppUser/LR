// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Interaction/LRInteractionStatics.h"

#include "Engine/OverlapResult.h"
#include "Gameplay/Systems/Interaction/LRInteractableTarget.h"

ULRInteractionStatics::ULRInteractionStatics()
: Super(FObjectInitializer::Get())
{
}

AActor* ULRInteractionStatics::GetActorFromInteractableTarget(
	TScriptInterface<ILRInteractableTarget> InteractableTarget)
{
	if (UObject* Object = InteractableTarget.GetObject())
	{
		if (AActor* Actor = Cast<AActor>(Object))
		{
			return Actor;
		}
		else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
		{
			return ActorComponent->GetOwner();
		}
		else
		{
			//TODO: Pop error message ?
			unimplemented();
		}
	}

	return nullptr;
}

void ULRInteractionStatics::GetInteractableTargetsFromActor(AActor* Actor,
	TArray<TScriptInterface<ILRInteractableTarget>>& OutInteractableTargets)
{
	// If the actor is directly interactable, return that.
	TScriptInterface<ILRInteractableTarget> InteractableActor(Actor);
	if (InteractableActor)
	{
		OutInteractableTargets.Add(InteractableActor);
	}

	// If the actor isn't interactable, it might have a component that has a interactable interface.
	TArray<UActorComponent*> InteractableComponents = Actor ? Actor->GetComponentsByInterface(ULRInteractableTarget::StaticClass()) : TArray<UActorComponent*>();
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		OutInteractableTargets.Add(TScriptInterface<ILRInteractableTarget>(InteractableComponent));
	}
}

void ULRInteractionStatics::AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults,
	TArray<TScriptInterface<ILRInteractableTarget>>& OutInteractableTargets)
{
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		TScriptInterface<ILRInteractableTarget> InteractableActor(Overlap.GetActor());
		if (InteractableActor)
		{
			OutInteractableTargets.AddUnique(InteractableActor);
		}

		TScriptInterface<ILRInteractableTarget> InteractableComponent(Overlap.GetComponent());
		if (InteractableComponent)
		{
			OutInteractableTargets.AddUnique(InteractableComponent);
		}
	}
}

void ULRInteractionStatics::AppendInteractableTargetsFromHitResult(const FHitResult& HitResult,
	TArray<TScriptInterface<ILRInteractableTarget>>& OutInteractableTargets)
{
	TScriptInterface<ILRInteractableTarget> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
	}

	TScriptInterface<ILRInteractableTarget> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		OutInteractableTargets.AddUnique(InteractableComponent);
	}
}

FText ULRInteractionStatics::GetInteractionVerb(EInteractionType Type)
{
	//TODO: Implement me
	return FText::GetEmpty();
}

bool ULRInteractionStatics::CheckAngleRequirement(const AActor* Interactor, const AActor* Target, float RequiredAngle)
{
	//TODO: Implement me
	return false;
}
