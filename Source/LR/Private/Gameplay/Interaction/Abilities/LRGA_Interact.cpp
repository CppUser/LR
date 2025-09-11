// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/Abilities/LRGA_Interact.h"

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Gameplay/Interaction/LRInteractableTarget.h"
#include "Gameplay/Interaction/LRInteractionOption.h"
#include "Gameplay/Interaction/LRInteractionStatics.h"
#include "Gameplay/Interaction/Attributes/LRInteractionAttribSet.h"
#include "Gameplay/Interaction/Tasks/LRGrantNearbyInteraction.h"
#include "Player/LRPlayerController.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Interaction_Activate, "Ability.Interaction.Activate");
UE_DEFINE_GAMEPLAY_TAG(TAG_INTERACTION_DURATION_MESSAGE, "Ability.Interaction.Duration.Message");


ULRGameplayAbility_Interact::ULRGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ActivationPolicy = EAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void ULRGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentInteractionMethod = DefaultInteractionMethod;

	
	switch (CurrentInteractionMethod)
	{
	case EInteractionMethod::Proximity:
		SetupProximityScanning();
		break;
	case EInteractionMethod::LineTrace:
		SetupLineTraceScanning();
		break;
	case EInteractionMethod::MouseOver:
		SetupMouseOverScanning();
		break;
	case EInteractionMethod::Combined:
		SetupProximityScanning();
		SetupLineTraceScanning();
		break;
	}

	if (bShowDebugInfo)
	{
		GetWorld()->GetTimerManager().SetTimer(DebugDrawTimerHandle, this, 
			&ULRGameplayAbility_Interact::DrawDebugInfo, 0.1f, true);
	}
	
	// UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	// if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	// {
	// 	UAbilityTask_GrantNearbyInteraction* Task = UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(this, InteractionScanRange, InteractionScanRate);
	// 	Task->ReadyForActivation();
	// }
}

void ULRGameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanupScanningTasks();
    
	if (bShowDebugInfo && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DebugDrawTimerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULRGameplayAbility_Interact::SetInteractionMethod(EInteractionMethod NewMethod)
{
}

bool ULRGameplayAbility_Interact::HasInteractionOfType(EInteractionType Type) const
{
	return false;
}

float ULRGameplayAbility_Interact::GetModifiedInteractionRange() const
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

float ULRGameplayAbility_Interact::GetInteractionRange() const
{
	return 0;
}

float ULRGameplayAbility_Interact::GetInteractionScanRate() const
{
	return 0;
}

float ULRGameplayAbility_Interact::GetInteractionAngle() const
{
	return 0;
}

void ULRGameplayAbility_Interact::FilterInteractionOptions(TArray<FInteractionOption>& Options) const
{
}

void ULRGameplayAbility_Interact::SortInteractionOptions(TArray<FInteractionOption>& Options) const
{
}

bool ULRGameplayAbility_Interact::IsActorInInteractionCone(const AActor* Target) const
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

void ULRGameplayAbility_Interact::SetupProximityScanning()
{
}

void ULRGameplayAbility_Interact::SetupLineTraceScanning()
{
}

void ULRGameplayAbility_Interact::SetupMouseOverScanning()
{
}

void ULRGameplayAbility_Interact::CleanupScanningTasks()
{
}

void ULRGameplayAbility_Interact::DrawDebugInfo() const
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	if (!World) return;

	FVector Location = Avatar->GetActorLocation();
	float Range = GetModifiedInteractionRange();
	float Angle = GetInteractionAngle();

	// Draw interaction sphere
	DrawDebugSphere(World, Location, Range, 32, FColor::Yellow, false, 0.1f);

	// Draw interaction cone
	if (Angle < 180.0f)
	{
		FVector Forward = Avatar->GetActorForwardVector();
		float HalfAngleRad = FMath::DegreesToRadians(Angle * 0.5f);
        
		// Draw cone lines
		FVector LeftDir = Forward.RotateAngleAxis(-Angle * 0.5f, FVector::UpVector);
		FVector RightDir = Forward.RotateAngleAxis(Angle * 0.5f, FVector::UpVector);
        
		DrawDebugLine(World, Location, Location + LeftDir * Range, FColor::Green, false, 0.1f, 0, 2.0f);
		DrawDebugLine(World, Location, Location + RightDir * Range, FColor::Green, false, 0.1f, 0, 2.0f);
		DrawDebugLine(World, Location, Location + Forward * Range, FColor::Red, false, 0.1f, 0, 2.0f);
	}

	// Draw current interaction options
	for (const auto& Option : CurrentOptions)
	{
		if (AActor* TargetActor = UInteractionStatics::GetActorFromInteractableTarget(Option.InteractableTarget))
		{
			FColor DebugColor = FColor::Blue;
			            
			DrawDebugSphere(World, TargetActor->GetActorLocation(), 50.0f, 16, DebugColor, false, 0.1f);
			DrawDebugLine(World, Location, TargetActor->GetActorLocation(), DebugColor, false, 0.1f);
		}
	}
}

void ULRGameplayAbility_Interact::UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions)
{
	// if (ALRPlayerController* PC = GetLRPlayerControllerFromActorInfo())
	// {
	// 	if (ULRIndicatorManagerComponent* IndicatorManager = ULRIndicatorManagerComponent::GetComponent(PC))
	// 	{
	// 		for (UIndicatorDescriptor* Indicator : Indicators)
	// 		{
	// 			IndicatorManager->RemoveIndicator(Indicator);
	// 		}
	// 		Indicators.Reset();
	//
	// 		for (const FInteractionOption& InteractionOption : InteractiveOptions)
	// 		{
	// 			AActor* InteractableTargetActor = UInteractionStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);
	//
	// 			TSoftClassPtr<UUserWidget> InteractionWidgetClass = 
	// 				InteractionOption.InteractionWidgetClass.IsNull() ? DefaultInteractionWidgetClass : InteractionOption.InteractionWidgetClass;
	//
	// 			UIndicatorDescriptor* Indicator = NewObject<UIndicatorDescriptor>();
	// 			Indicator->SetDataObject(InteractableTargetActor);
	// 			Indicator->SetSceneComponent(InteractableTargetActor->GetRootComponent());
	// 			Indicator->SetIndicatorClass(InteractionWidgetClass);
	// 			IndicatorManager->AddIndicator(Indicator);
	//
	// 			Indicators.Add(Indicator);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		//TODO This should probably be a noisy warning.  Why are we updating interactions on a PC that can never do anything with them?
	// 	}
	// }

	CurrentOptions = InteractiveOptions;
}

void ULRGameplayAbility_Interact::TriggerInteraction(int32 OptionIndex)
{
}

void ULRGameplayAbility_Interact::TriggerInteractionByType(EInteractionType Type)
{
}

// void ULRGameplayAbility_Interact::TriggerInteraction()
// {
// 	if (CurrentOptions.Num() == 0)
// 	{
// 		return;
// 	}
//
// 	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
// 	if (AbilitySystem)
// 	{
// 		const FInteractionOption& InteractionOption = CurrentOptions[0];
//
// 		AActor* Instigator = GetAvatarActorFromActorInfo();
// 		AActor* InteractableTargetActor = UInteractionStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);
//
// 		// Allow the target to customize the event data we're about to pass in, in case the ability needs custom data
// 		// that only the actor knows.
// 		FGameplayEventData Payload;
// 		Payload.EventTag = TAG_Ability_Interaction_Activate;
// 		Payload.Instigator = Instigator;
// 		Payload.Target = InteractableTargetActor;
//
// 		// If needed we allow the interactable target to manipulate the event data so that for example, a button on the wall
// 		// may want to specify a door actor to execute the ability on, so it might choose to override Target to be the
// 		// door actor.
// 		InteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, Payload);
//
// 		// Grab the target actor off the payload we're going to use it as the 'avatar' for the interaction, and the
// 		// source InteractableTarget actor as the owner actor.
// 		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));
//
// 		// The actor info needed for the interaction.
// 		FGameplayAbilityActorInfo ActorInfo;
// 		ActorInfo.InitFromActor(InteractableTargetActor, TargetActor, InteractionOption.TargetAbilitySystem);
//
// 		// Trigger the ability using event tag.
// 		const bool bSuccess = InteractionOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
// 			InteractionOption.TargetInteractionAbilityHandle,
// 			&ActorInfo,
// 			TAG_Ability_Interaction_Activate,
// 			&Payload,
// 			*InteractionOption.TargetAbilitySystem
// 		);
// 	}
// }
