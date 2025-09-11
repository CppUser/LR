// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/Abilities/LRGA_Interact.h"
#include "NativeGameplayTags.h"
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
}

void ULRGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//TODO: Need to properly determine interaction Method
	CurrentInteractionMethod = DefaultInteractionMethod;
	
	if (bShowDebugInfo)
	{
		GetWorld()->GetTimerManager().SetTimer(DebugDrawTimerHandle, this, 
			&ThisClass::DrawDebugInfo, 0.1f, true);
	}
	
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem)
	{
		ULRScanForInteractables* Task = ULRScanForInteractables::ScanForInteractables(this,
			GetModifiedInteractionRange(),
			GetInteractionScanRate(),
			GetInteractionAngle(),
			true, //TODO: Need to implement
			true, //TODO Need to implement properly IsActorInInteractionCone checks if actor in cone
			CurrentInteractionMethod); 
		Task->ReadyForActivation();
	}
	
}

void ULRGA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULRGA_Interact::UpdateInteractionOption(const TArray<FLRInteractionOption>& NewOptions)
{
}

void ULRGA_Interact::TriggerInteraction(int32 OptionIndex)
{
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

void ULRGA_Interact::DrawDebugInfo() const
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
		if (AActor* TargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(Option.InteractableTarget))
		{
			FColor DebugColor = FColor::Blue;
            
			DrawDebugSphere(World, TargetActor->GetActorLocation(), 50.0f, 16, DebugColor, false, 0.1f);
			DrawDebugLine(World, Location, TargetActor->GetActorLocation(), DebugColor, false, 0.1f);
            
			// Draw interaction type text
			FString TypeText = UEnum::GetValueAsString(Option.InteractionType);
			DrawDebugString(World, TargetActor->GetActorLocation() + FVector(0, 0, 100), 
				TypeText, nullptr, DebugColor, 0.1f);
		}
	}
}

