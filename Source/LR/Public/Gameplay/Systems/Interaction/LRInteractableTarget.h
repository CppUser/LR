#pragma once

#include "LRInteractionOption.h"
#include "LRInteractionQuery.h"
#include "LRInteractionStatics.h"
#include "Abilities/GameplayAbility.h"
#include "LRInteractableTarget.generated.h"

class ILRInteractableTarget;

class FLRInteractionOptionBuilder
{
public:
	FLRInteractionOptionBuilder(TScriptInterface<ILRInteractableTarget> InterfaceTargetScope, TArray<FLRInteractionOption>& InteractOptions, const FLRInteractionQuery& Query)
	   : Scope(InterfaceTargetScope)
	   , Options(InteractOptions)
	   , InteractionQuery(Query) {}

	void AddInteractionOption(const FLRInteractionOption& Option)
	{
		
		FLRInteractionOption ModifiedOption = Option;
        
		
		if (ModifiedOption.Priority == 0)
		{
			if (AActor* Instigator = InteractionQuery.RequestingAvatar.Get())
			{
				if (AActor* TargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(Scope))
				{
					const float Distance = FVector::Distance(Instigator->GetActorLocation(), TargetActor->GetActorLocation());
					ModifiedOption.Priority = FMath::RoundToInt(1000.0f / FMath::Max(1.0f, Distance));
				}
			}
		}
        
		ModifiedOption.InteractableTarget = Scope;
        
		Options.Add(ModifiedOption);
	}

	const FLRInteractionQuery& GetQuery() const { return InteractionQuery; }

private:
	TScriptInterface<ILRInteractableTarget> Scope;
	TArray<FLRInteractionOption>& Options;
	const FLRInteractionQuery& InteractionQuery;
};


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class ULRInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

/**  */
class ILRInteractableTarget
{
	GENERATED_BODY()

public:
	/**  */
	virtual void GatherInteractionOptions(const FLRInteractionQuery& InteractQuery, FLRInteractionOptionBuilder& OptionBuilder) = 0;

	/**  */
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) { }
};
