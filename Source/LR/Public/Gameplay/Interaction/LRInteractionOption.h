#pragma once
#include "LRInteractionTypes.h"
#include "Abilities/GameplayAbility.h"
#include "LRInteractionOption.generated.h"

class IInteractableTarget;
class UUserWidget;

USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionType InteractionType = EInteractionType::Collect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionMethod InteractionMethod = EInteractionMethod::Combined;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInteractionRequirement Requirements;
	
	/** The interactable target */
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractableTarget> InteractableTarget;

	/** Simple text the interaction might return */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	/** Simple sub-text the interaction might return */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InteractionIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* InteractionSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeInterrupted = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InteractionTag;

	// METHODS OF INTERACTION
	//--------------------------------------------------------------

	// 1) Place an ability on the avatar that they can activate when they perform interaction.

	/** The ability to grant the avatar when they get near interactable objects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	// - OR -

	// 2) Allow the object we're interacting with to have its own ability system and interaction ability, that we can activate instead.

	/** The ability system on the target that can be used for the TargetInteractionHandle and sending the event, if needed. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem = nullptr;

	/** The ability spec to activate on the object for this option. */
	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;

	// UI
	//--------------------------------------------------------------

	/** The widget to show for this kind of interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

	//--------------------------------------------------------------

public:
	FORCEINLINE bool operator==(const FInteractionOption& Other) const
	{
		return InteractableTarget == Other.InteractableTarget &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant&&
			TargetAbilitySystem == Other.TargetAbilitySystem &&
			TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle &&
			InteractionWidgetClass == Other.InteractionWidgetClass &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText);
	}

	FORCEINLINE bool operator!=(const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FInteractionOption& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}
};


UCLASS()
class LR_API ULRInteractionHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	static FText GetInteractionVerb(EInteractionType Type)
	{
		return FText();
	}

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	static bool CheckAngleRequirement(const AActor* Interactor, const AActor* Target, float RequiredAngle)
	{
		return false;
	}
};


