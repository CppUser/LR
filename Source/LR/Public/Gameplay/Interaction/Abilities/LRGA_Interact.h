// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AbilitySystem/Core/LRGameplayAbility.h"


#include "LRGA_Interact.generated.h"

struct FInteractionOption;
class ULRIndicatorDescriptor;

UCLASS(Abstract)
class ULRGameplayAbility_Interact : public ULRGameplayAbility
{
	GENERATED_BODY()

public:

	ULRGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

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
};
