// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/LRAttributeSet.h"

#include "AbilitySystem/Core/LRAbilitySystemComponent.h"

ULRAttributeSet::ULRAttributeSet()
{
}

UWorld* ULRAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

ULRAbilitySystemComponent* ULRAttributeSet::GetLRAbilitySystemComponent() const
{
	return Cast<ULRAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
