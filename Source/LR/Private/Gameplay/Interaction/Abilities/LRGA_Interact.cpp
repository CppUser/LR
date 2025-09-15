// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/Abilities/LRGA_Interact.h"
#include "NativeGameplayTags.h"
#include "Gameplay/Interaction/LRInteractableTarget.h"
#include "Gameplay/Interaction/LRInteractionAttribSet.h"
#include "Gameplay/Interaction/LRInteractionOption.h"
#include "Gameplay/Interaction/LRInteractionStatics.h"
#include "Gameplay/Interaction/Tasks/LRScanForInteractables.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Interaction_Activate, "Ability.Interaction.Activate");
UE_DEFINE_GAMEPLAY_TAG(TAG_INTERACTION_DURATION_MESSAGE, "Ability.Interaction.Duration.Message");


ULRGA_Interact::ULRGA_Interact(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ActivationPolicy = EAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	bServerRespectsRemoteAbilityCancellation = false;
}

void ULRGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//TODO: Need to properly determine interaction Method
	CurrentInteractionMethod = DefaultInteractionMethod;
	
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem)
	{
		//TODO: Should i handle this task simply for proximity and grant required ability ?
		ULRScanForInteractables* Task = ULRScanForInteractables::ScanForInteractables(this,
			GetModifiedInteractionRange(),
			GetInteractionScanRate(),
			GetInteractionAngle(),
			true, //TODO: Need to implement
			true, //TODO Need to implement properly IsActorInInteractionCone checks if actor in cone
			CurrentInteractionMethod,
			bShowDebugInfo);
		
		if (Task)
		{
			// Bind to the delegate to receive updates
			Task->OnInteractablesFound.AddDynamic(this, &ULRGA_Interact::OnInteractablesUpdated);
			Task->ReadyForActivation();
		}
	}
	
}

void ULRGA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULRGA_Interact::UpdateInteractionOption(const TArray<FLRInteractionOption>& NewOptions)
{
	CurrentOptions = NewOptions;
}

void ULRGA_Interact::TriggerInteraction(int32 OptionIndex)
{
	if (!CurrentOptions.IsValidIndex(OptionIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("TriggerInteraction: Invalid option index %d"), OptionIndex);
		return;
	}

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("TriggerInteraction: No ability system component"));
		return;
	}

	const FLRInteractionOption& InteractionOption = CurrentOptions[OptionIndex];
	
	AActor* Instigator = GetAvatarActorFromActorInfo();
	AActor* InteractableTargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);

	if (!InteractableTargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TriggerInteraction: No target actor"));
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = TAG_Ability_Interaction_Activate;
	Payload.Instigator = Instigator;
	Payload.Target = InteractableTargetActor;

	// Allow the target to customize the event data
	if (InteractionOption.InteractableTarget)
	{
		InteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, Payload);
	}

	bool bInteractionTriggered = false;

	// Try to activate ability on target
	if (InteractionOption.TargetAbilitySystem && InteractionOption.TargetInteractionAbilityHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("TriggerInteraction: Triggering ability on target %s"), *GetNameSafe(InteractableTargetActor));
		
		// Create actor info for the target
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableTargetActor, InteractableTargetActor, InteractionOption.TargetAbilitySystem);
		
		// Try to activate the ability
		bInteractionTriggered = InteractionOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
			InteractionOption.TargetInteractionAbilityHandle,
			&ActorInfo,
			TAG_Ability_Interaction_Activate,
			&Payload,
			*InteractionOption.TargetAbilitySystem
		);
		
		if (!bInteractionTriggered)
		{
			UE_LOG(LogTemp, Warning, TEXT("TriggerInteraction: Failed to trigger ability on target"));
		}
	}

	if (InteractionOption.TargetAbilitySystem && InteractionOption.TargetInteractionAbilityHandle.IsValid())
	{
		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));
        
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableTargetActor, TargetActor, InteractionOption.TargetAbilitySystem);

		InteractionOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
			InteractionOption.TargetInteractionAbilityHandle,
			&ActorInfo,
			TAG_Ability_Interaction_Activate,
			&Payload,
			*InteractionOption.TargetAbilitySystem
		);
	}
	// Otherwise, if there's an ability to grant, try to activate it
	else if (InteractionOption.InteractionAbilityToGrant)
	{
		FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(InteractionOption.InteractionAbilityToGrant);
		if (Spec)
		{
			AbilitySystem->TryActivateAbility(Spec->Handle);
		}
	}
}

int32 ULRGA_Interact::GetCurrentOptionIndex() const
{
	return 0;
}

float ULRGA_Interact::GetModifiedInteractionRange() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return BaseInteractionRange;

	const ULRInteractionAttribSet* InteractionSet = ASC->GetSet<ULRInteractionAttribSet>();
	if (InteractionSet)
	{
		float Range = InteractionSet->GetInteractionRange();
		float Multiplier = InteractionSet->GetInteractionRangeMultiplier();
		return Range * Multiplier;
	}
    
	return BaseInteractionRange;
}

float ULRGA_Interact::GetInteractionScanRate() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return BaseInteractionScanRate;

	const ULRInteractionAttribSet* InteractionSet = ASC->GetSet<ULRInteractionAttribSet>();
	if (InteractionSet)
	{
		return InteractionSet->GetInteractionScanRate();
	}
    
	return BaseInteractionScanRate;
}

float ULRGA_Interact::GetInteractionAngle() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return BaseInteractionAngle;

	const ULRInteractionAttribSet* InteractionSet = ASC->GetSet<ULRInteractionAttribSet>();
	if (InteractionSet)
	{
		return InteractionSet->GetInteractionAngle();
	}
    
	return BaseInteractionAngle;
}

bool ULRGA_Interact::IsActorInInteractionCone(const AActor* Target) const
{
	if (!Target) return false;
    
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return false;

	FVector ToTarget = Target->GetActorLocation() - Avatar->GetActorLocation();
	ToTarget.Z = 0; // Ignore vertical difference for cone check
	ToTarget.Normalize();

	FVector Forward = Avatar->GetActorForwardVector();
	Forward.Z = 0;
	Forward.Normalize();

	float DotProduct = FVector::DotProduct(Forward, ToTarget);
	float AngleRadians = FMath::Acos(DotProduct);
	float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	float ConeHalfAngle = GetInteractionAngle() * 0.5f;
	return AngleDegrees <= ConeHalfAngle;
}

void ULRGA_Interact::OnInteractablesUpdated(const TArray<FLRInteractionOption>& NewOptions)
{
	UpdateInteractionOption(NewOptions);
}



