// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/LRGameplayAbility.h"
#include "Gameplay/Interaction/LRInteractionOption.h"
#include "LRGA_Interact.generated.h"

struct FLRInteractionOption;
/**
 * 
 */
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

	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void UpdateInteractionOption(const TArray<FLRInteractionOption>& NewOptions);

	UFUNCTION(BlueprintCallable, Category = "LR|Interaction")
	void TriggerInteraction(int32 OptionIndex = 0);

	int32 GetCurrentOptionIndex() const;
private:
	float GetModifiedInteractionRange() const;
	float GetInteractionScanRate() const;
	float GetInteractionAngle() const;
	bool IsActorInInteractionCone(const AActor* Target) const;

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

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> MultiOptionWidgetClass;

	EInteractionMethod CurrentInteractionMethod;
	
};
