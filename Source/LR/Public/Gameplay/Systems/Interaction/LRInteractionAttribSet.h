// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Core/LRAttributeSet.h"
#include "LRInteractionAttribSet.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LR_API ULRInteractionAttribSet : public ULRAttributeSet
{
	GENERATED_BODY()
public:
	ULRInteractionAttribSet();

	ATTRIBUTE_ACCESSORS(ULRInteractionAttribSet, InteractionRange);
	ATTRIBUTE_ACCESSORS(ULRInteractionAttribSet, InteractionScanRate);
	ATTRIBUTE_ACCESSORS(ULRInteractionAttribSet, InteractionAngle);
	ATTRIBUTE_ACCESSORS(ULRInteractionAttribSet, InteractionRangeMultiplier);

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
private:
	// Base interaction range in units
	UPROPERTY(BlueprintReadOnly, Category = "LR|Interaction", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData InteractionRange;

	// How often to scan for interactables (lower is faster)
	UPROPERTY(BlueprintReadOnly, Category = "LR|Interaction", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData InteractionScanRate;

	// Angle in degrees for interaction cone (0-180, where 180 is full circle)
	UPROPERTY(BlueprintReadOnly, Category = "LR|Interaction", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData InteractionAngle;

	// Multiplier for environmental effects (fog, darkness, etc.)
	UPROPERTY(BlueprintReadOnly, Category = "LR|Interaction", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData InteractionRangeMultiplier;

};
