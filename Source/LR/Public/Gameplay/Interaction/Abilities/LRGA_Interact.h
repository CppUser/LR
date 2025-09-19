// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/LRGameplayAbility.h"
#include "Gameplay/Interaction/LRInteractionOption.h"
#include "LRGA_Interact.generated.h"

class ULRScanForInteractables;
struct FLRInteractionOption;

USTRUCT(BlueprintType)
struct FActiveInteraction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FLRInteractionOption Option;

	UPROPERTY(BlueprintReadWrite)
	float HoldStartTime = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bIsHolding = false;

	UPROPERTY(BlueprintReadWrite)
	bool bCompleted = false;

	void Reset()
	{
		HoldStartTime = 0.0f;
		bIsHolding = false;
		bCompleted = false;
	}
};


UCLASS()
class LR_API ULRGA_Interact : public ULRGameplayAbility
{
	GENERATED_BODY()
public:
	ULRGA_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void UpdateInteractionOption(const TArray<FLRInteractionOption>& NewOptions);
 
	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void TriggerInteraction(int32 OptionIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void OnTimedInteractionComplete(int32 OptionIndex = 0);
	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	float GetInteractionProgress() const;

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	bool IsHoldingInteraction() const { return ActiveInteraction.bIsHolding; }

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	FLRInteractionOption GetCurrentInteractionOption() const;
	
	int32 GetCurrentOptionIndex() const;
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionStarted(const FLRInteractionOption& Option);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionProgress(float Progress, const FLRInteractionOption& Option);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionCancelled(const FLRInteractionOption& Option);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionCompleted(const FLRInteractionOption& Option);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionOptionsUpdated(const TArray<FLRInteractionOption>& Options);

private:
	float GetModifiedInteractionRange() const;
	float GetInteractionScanRate() const;
	float GetInteractionAngle() const;
	bool IsActorInInteractionCone(const AActor* Target) const;

	void StartHoldInteraction(int32 OptionIndex);
	void UpdateHoldInteraction();
	void CancelHoldInteraction();
	void CompleteHoldInteraction();
	UFUNCTION()
	void HoldInteractionTick();
		
	UFUNCTION()
	void OnInteractablesUpdated(const TArray<FLRInteractionOption>& NewOptions);
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FLRInteractionOption> CurrentOptions;

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

	UPROPERTY(EditDefaultsOnly, Category = "Interaction Settings")
	float HoldTickRate = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> MultiOptionWidgetClass;

	EInteractionMethod CurrentInteractionMethod;

	UPROPERTY()
	ULRScanForInteractables* ScanTask;

	FActiveInteraction ActiveInteraction;
	FTimerHandle HoldTimerHandle;
    
	bool bIsInputPressed = false;
};
