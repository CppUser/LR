// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AbilitySystem/Core/LRGameplayAbility.h"
#include "Gameplay/Interaction/LRInteractionTypes.h"
#include "LRGA_Interact.generated.h"

struct FInteractionOption;
class ULRIndicatorDescriptor;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionOptionsUpdated, const TArray<FInteractionOption>&, Options);


UCLASS(Abstract)
class ULRGameplayAbility_Interact : public ULRGameplayAbility
{
	GENERATED_BODY()

public:

	ULRGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
			const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions);
	
	UFUNCTION(BlueprintCallable)
	void TriggerInteraction(int32 OptionIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void TriggerInteractionByType(EInteractionType Type);

	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void SetInteractionMethod(EInteractionMethod NewMethod);

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	TArray<FInteractionOption> GetAvailableInteractions() const { return CurrentOptions; }

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	bool HasInteractionOfType(EInteractionType Type) const;

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	float GetModifiedInteractionRange() const;

protected:
	// Get interaction attributes from ASC
	float GetInteractionRange() const;
	float GetInteractionScanRate() const;
	float GetInteractionAngle() const;

	// Filter options based on requirements
	void FilterInteractionOptions(TArray<FInteractionOption>& Options) const;

	// Sort options by priority and distance
	void SortInteractionOptions(TArray<FInteractionOption>& Options) const;

	// Check if actor is within interaction cone
	bool IsActorInInteractionCone(const AActor* Target) const;

	// Handle different interaction methods
	void SetupProximityScanning();
	void SetupLineTraceScanning();
	void SetupMouseOverScanning();
	void CleanupScanningTasks();
public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractionOptionsUpdated OnInteractionOptionsUpdated;

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FInteractionOption> CurrentOptions;

	// UPROPERTY()
	// TArray<TObjectPtr<ULRIndicatorDescriptor>> Indicators;

protected:

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	EInteractionMethod DefaultInteractionMethod = EInteractionMethod::Combined;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	float BaseInteractionRange = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	float BaseInteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	float BaseInteractionAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	bool bShowDebugInfo = false;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	bool bAllowMultipleInteractions = false;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	int32 MaxSimultaneousOptions = 3;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> MultiOptionWidgetClass;

private:
	UPROPERTY()
	class UAbilityTask_GrantNearbyInteraction* ProximityTask;

	UPROPERTY()
	class UAbilityTask_WaitForInteractableTargets* LineTraceTask;

	UPROPERTY()
	class UAbilityTask* MouseOverTask;

	EInteractionMethod CurrentInteractionMethod;
    
	FTimerHandle DebugDrawTimerHandle;
	void DrawDebugInfo() const;
};
