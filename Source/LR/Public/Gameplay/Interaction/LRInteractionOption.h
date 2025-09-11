#pragma once

#include "Abilities/GameplayAbility.h"
#include "LRInteractionOption.generated.h"

class ILRInteractableTarget;


UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None            UMETA(DisplayName = "None"),
	Collect         UMETA(DisplayName = "Collect/Loot"),
	Talk            UMETA(DisplayName = "Talk"),
	Use             UMETA(DisplayName = "Use/Activate"),
	Examine         UMETA(DisplayName = "Examine"),
	Mount           UMETA(DisplayName = "Mount/Ride"),
	Open            UMETA(DisplayName = "Open"),
	Custom          UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class EInteractionMethod : uint8
{
	Proximity       UMETA(DisplayName = "Proximity Based"),
	LineTrace       UMETA(DisplayName = "Looking At"),
	MouseOver       UMETA(DisplayName = "Mouse Over"),
	Combined        UMETA(DisplayName = "Combined")
};

USTRUCT(BlueprintType)
struct FInteractionRequirement
{
	GENERATED_BODY()

	// Required tags on the interactor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	// Tags that block this interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTags;

	// Minimum distance for interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistance = 0.0f;

	// Maximum distance for interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 500.0f;

	// Required angle (0 = must be behind, 90 = side, 180 = front)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RequiredAngle = -1.0f; // -1 means any angle

	// Is line of sight required?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequiresLineOfSight = true;
};


USTRUCT(BlueprintType)
struct FLRInteractionOption
{
	GENERATED_BODY()

public:
	/** The interactable target */
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<ILRInteractableTarget> InteractableTarget;
	
	// Type of interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionType InteractionType = EInteractionType::Collect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionMethod InteractionMethod = EInteractionMethod::Combined;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInteractionRequirement Requirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InteractionIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* InteractionSound = nullptr;
	
	
	/** Simple text the interaction might return */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	/** Simple sub-text the interaction might return */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubText;

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
	FORCEINLINE bool operator==(const FLRInteractionOption& Other) const
	{
		return InteractableTarget == Other.InteractableTarget &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant&&
			TargetAbilitySystem == Other.TargetAbilitySystem &&
			TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle &&
			InteractionWidgetClass == Other.InteractionWidgetClass &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText);
	}

	FORCEINLINE bool operator!=(const FLRInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FLRInteractionOption& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}
};

