#pragma once

#include "Abilities/GameplayAbility.h"
#include "LRInteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FLRInteractionQuery
{
	GENERATED_BODY()

public:
	/** The requesting pawn. */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingAvatar;

	/** Allow us to specify a controller - does not need to match the owner of the requesting avatar. */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;

	/** A generic UObject to shove in extra data required for the interaction */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;

	/** Game state tags that can be used to filter interactions */
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer GameStateTags;
	
	/** The current interaction being performed (if any) */
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag ActiveInteractionTag;

	UPROPERTY(BlueprintReadWrite)
	FVector InteractionOrigin;

	UPROPERTY(BlueprintReadWrite)
	FVector InteractionDirection;

	FGuid GetQueryId() const { return QueryId; }
	
private:
	FGuid QueryId = FGuid::NewGuid();
};