// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LRActor.h"

#include "AbilitySystemGlobals.h"
#include "Gameplay/Interaction/LRInteractionQuery.h"
#include "UObject/FastReferenceCollector.h"

//////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////
ALRActor::ALRActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ALRActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(StaticGameplayTags);
}
#if WITH_EDITOR
bool ALRActor::CanEditChange(const FProperty* InProperty) const
{
	// Prevent editing of the other tags property
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AActor, Tags))
	{
		return false;
	}
	
	return Super::CanEditChange(InProperty);
}
#endif
void ALRActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////
///             ALRInteractableActor
/////////////////////////////////////////////////////////
ALRInteractableActor::ALRInteractableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FInteractionOption DefaultOption;
	DefaultOption.InteractionType = EInteractionType::Use;
	DefaultOption.Text = FText::FromString("Interact");
	DefaultOption.InteractionMethod = EInteractionMethod::Combined;
	DefaultOption.Priority = 0;
	AvailableInteractions.Add(DefaultOption);
}

void ALRInteractableActor::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	AActor* Interactor = InteractQuery.RequestingAvatar.Get();
	if (!Interactor) return;

	for (const FInteractionOption& Option : AvailableInteractions)
	{
		// Check if this interaction type is currently available
		bool bCanProvide = false;
        
		switch (Option.InteractionType)
		{
		case EInteractionType::Collect:
			//TODO:bCanProvide = bIsLootable && LootItems.Num() > 0;
			break;
		case EInteractionType::Use:
			bCanProvide = bIsUsable;
			break;
		default:
			bCanProvide = true;
			break;
		}

		if (bCanProvide && Option.Requirements.CheckRequirements(Interactor, this))
		{
			FInteractionOption BaseOption;
			BaseOption.Text = Option.Text;
			BaseOption.SubText = Option.SubText;
			BaseOption.InteractionAbilityToGrant = Option.InteractionAbilityToGrant;
			BaseOption.InteractionWidgetClass = Option.InteractionWidgetClass;
            
			// If this actor has an ability system, use it for the interaction
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this))
			{
				BaseOption.TargetAbilitySystem = ASC;
				// You would set up the ability handle here based on your granted abilities
			}
            
			OptionBuilder.AddInteractionOption(BaseOption);
		}
	}
	
}

void ALRInteractableActor::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	IInteractableTarget::CustomizeInteractionEventData(InteractionEventTag, InOutEventData);
}

bool ALRInteractableActor::CanProvideInteraction(EInteractionType Type, const AActor* Interactor) const
{
	for (const FInteractionOption& Option : AvailableInteractions)
	{
		if (Option.InteractionType == Type)
		{
			return Option.Requirements.CheckRequirements(Interactor, this);
		}
	}
	return false;
}

