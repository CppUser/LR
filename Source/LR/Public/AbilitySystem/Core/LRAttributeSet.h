// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "AttributeSet.h"
#include "LRAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class ULRAbilitySystemComponent;
struct FGameplayEffectSpec;

DECLARE_MULTICAST_DELEGATE_SixParams(FLRAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
                                     const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/,
                                     float /*OldValue*/, float /*NewValue*/);


UCLASS()
class LR_API ULRAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	ULRAttributeSet();

	UWorld* GetWorld() const override;

	ULRAbilitySystemComponent* GetLRAbilitySystemComponent() const;
};
