// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Interaction/LRInteractionAttribSet.h"

ULRInteractionAttribSet::ULRInteractionAttribSet()
: InteractionRange(500.0f)
, InteractionScanRate(0.1f)
, InteractionAngle(90.0f)
, InteractionRangeMultiplier(1.0f)
{
}

void ULRInteractionAttribSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetInteractionRangeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 50.0f, 2000.0f);
	}
	else if (Attribute == GetInteractionScanRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.01f, 1.0f);
	}
	else if (Attribute == GetInteractionAngleAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 15.0f, 180.0f);
	}
	else if (Attribute == GetInteractionRangeMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 2.0f);
	}
}

void ULRInteractionAttribSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
