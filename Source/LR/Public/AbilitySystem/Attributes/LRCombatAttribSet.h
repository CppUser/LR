// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Core/LRAttributeSet.h"
#include "LRCombatAttribSet.generated.h"


UCLASS(BlueprintType)
class LR_API ULRCombatAttribSet : public ULRAttributeSet
{
	GENERATED_BODY()
public:
	ULRCombatAttribSet();

	ATTRIBUTE_ACCESSORS(ULRCombatAttribSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(ULRCombatAttribSet, BaseHeal);

private:
	UPROPERTY(BlueprintReadOnly, Category = "LR|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
