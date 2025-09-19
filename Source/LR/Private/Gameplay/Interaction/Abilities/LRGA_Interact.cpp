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
		ScanTask = ULRScanForInteractables::ScanForInteractables(this,
			GetModifiedInteractionRange(),
			GetInteractionScanRate(),
			GetInteractionAngle(),
			true, //TODO: Need to implement
			true, //TODO Need to implement properly IsActorInInteractionCone checks if actor in cone
			CurrentInteractionMethod,
			bShowDebugInfo);
		
		if (ScanTask)
		{
			ScanTask->OnInteractablesFound.AddDynamic(this, &ULRGA_Interact::OnInteractablesUpdated);
			ScanTask->ReadyForActivation();
		}
	}
	
}

void ULRGA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActiveInteraction.bIsHolding)
	{
		CancelHoldInteraction();
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	}

	if (ScanTask)
	{
		ScanTask->EndTask();
		ScanTask = nullptr;
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULRGA_Interact::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bIsInputPressed = true;
    
	if (CurrentOptions.Num() > 0)
	{
		StartHoldInteraction(0);
	}
}

void ULRGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bIsInputPressed = false;
    
	if (ActiveInteraction.bIsHolding)
	{
		const FLRInteractionOption& Option = ActiveInteraction.Option;
        
		if (Option.InteractionDuration > 0.0f)
		{
			if (ActiveInteraction.bCompleted)
			{
				CompleteHoldInteraction();
			}
			else
			{
				CancelHoldInteraction();
			}
		}
		else
		{
			CompleteHoldInteraction();
		}
	}
}

void ULRGA_Interact::UpdateInteractionOption(const TArray<FLRInteractionOption>& NewOptions)
{
	CurrentOptions = NewOptions;
	OnInteractionOptionsUpdated(NewOptions);
    
	if (!ActiveInteraction.bIsHolding && bIsInputPressed && CurrentOptions.Num() > 0)
	{
		StartHoldInteraction(0);
	}
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
	Payload.EventMagnitude = InteractionOption.InteractionDuration;

	if (InteractionOption.InteractableTarget)
	{
		InteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, Payload);
	}

	bool bInteractionTriggered = false;

	if (InteractionOption.TargetAbilitySystem && InteractionOption.TargetInteractionAbilityHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("TriggerInteraction: Triggering ability on target %s"), *GetNameSafe(InteractableTargetActor));
        
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableTargetActor, InteractableTargetActor, InteractionOption.TargetAbilitySystem);
        
		bInteractionTriggered = InteractionOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
			InteractionOption.TargetInteractionAbilityHandle,
			&ActorInfo,
			TAG_Ability_Interaction_Activate,
			&Payload,
			*InteractionOption.TargetAbilitySystem
		);
	}
	else if (InteractionOption.InteractionAbilityToGrant)
	{
		UE_LOG(LogTemp, Log, TEXT("TriggerInteraction: Triggering ability on avatar with event data"));
        
		FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(InteractionOption.InteractionAbilityToGrant);
		if (Spec)
		{
			bInteractionTriggered = AbilitySystem->TriggerAbilityFromGameplayEvent(
				Spec->Handle,
				AbilitySystem->AbilityActorInfo.Get(),
				TAG_Ability_Interaction_Activate,
				&Payload,
				*AbilitySystem
			);
            
			if (!bInteractionTriggered)
			{
				UE_LOG(LogTemp, Warning, TEXT("TriggerInteraction: Fallback to regular activation"));
				bInteractionTriggered = AbilitySystem->TryActivateAbility(Spec->Handle);
			}
		}
	}
    
	if (bInteractionTriggered)
	{
		UE_LOG(LogTemp, Log, TEXT("TriggerInteraction: Successfully triggered interaction"));
	}
	
}

void ULRGA_Interact::OnTimedInteractionComplete(int32 OptionIndex)
{
	CompleteHoldInteraction();
}

float ULRGA_Interact::GetInteractionProgress() const
{
	if (!ActiveInteraction.bIsHolding || ActiveInteraction.Option.InteractionDuration <= 0.0f)
	{
		return 0.0f;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float HoldTime = CurrentTime - ActiveInteraction.HoldStartTime;
	return FMath::Clamp(HoldTime / ActiveInteraction.Option.InteractionDuration, 0.0f, 1.0f);
}

FLRInteractionOption ULRGA_Interact::GetCurrentInteractionOption() const
{
	if (ActiveInteraction.bIsHolding)
	{
		return ActiveInteraction.Option;
	}
    
	if (CurrentOptions.Num() > 0)
	{
		return CurrentOptions[0];
	}
    
	return FLRInteractionOption();
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

void ULRGA_Interact::StartHoldInteraction(int32 OptionIndex)
{
	if (!CurrentOptions.IsValidIndex(OptionIndex))
	{
		return;
	}

	ActiveInteraction.Option = CurrentOptions[OptionIndex];
	ActiveInteraction.HoldStartTime = GetWorld()->GetTimeSeconds();
	ActiveInteraction.bIsHolding = true;
	ActiveInteraction.bCompleted = false;

	OnInteractionStarted(ActiveInteraction.Option);

	if (ActiveInteraction.Option.InteractionDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			HoldTimerHandle,
			this,
			&ULRGA_Interact::HoldInteractionTick,
			HoldTickRate,
			true
		);
	}
}

void ULRGA_Interact::UpdateHoldInteraction()
{
	if (!ActiveInteraction.bIsHolding)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float HoldTime = CurrentTime - ActiveInteraction.HoldStartTime;
	const float Duration = ActiveInteraction.Option.InteractionDuration;

	if (Duration > 0.0f)
	{
		const float Progress = FMath::Clamp(HoldTime / Duration, 0.0f, 1.0f);
        
		OnInteractionProgress(Progress, ActiveInteraction.Option);

		if (Progress >= 1.0f && !ActiveInteraction.bCompleted)
		{
			ActiveInteraction.bCompleted = true;
            
			if (!ActiveInteraction.Option.bCanBeInterrupted)
			{
				CompleteHoldInteraction();
			}
		}
	}
}

void ULRGA_Interact::CancelHoldInteraction()
{
	if (!ActiveInteraction.bIsHolding)
	{
		return;
	}

	OnInteractionCancelled(ActiveInteraction.Option);
    
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	ActiveInteraction.Reset();
}

void ULRGA_Interact::CompleteHoldInteraction()
{
	if (!ActiveInteraction.bIsHolding)
	{
		return;
	}

	int32 OptionIndex = CurrentOptions.IndexOfByPredicate([this](const FLRInteractionOption& Option)
	{
		return Option == ActiveInteraction.Option;
	});

	if (OptionIndex != INDEX_NONE)
	{
		TriggerInteraction(OptionIndex);
		OnInteractionCompleted(ActiveInteraction.Option);
	}

	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	ActiveInteraction.Reset();
}

void ULRGA_Interact::HoldInteractionTick()
{
	if (!ActiveInteraction.bIsHolding)
	{
		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
		return;
	}

	UpdateHoldInteraction();
}

void ULRGA_Interact::OnInteractablesUpdated(const TArray<FLRInteractionOption>& NewOptions)
{
	UpdateInteractionOption(NewOptions);
}



