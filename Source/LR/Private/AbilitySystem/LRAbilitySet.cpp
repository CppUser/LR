﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/AbilitySystem/LRAbilitySet.h"

#include "AbilitySystem/Core/LRAbilitySystemComponent.h"


void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(ULRAbilitySystemComponent* ASC)
{
	check(ASC);

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}
	
	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}


void ULRAbilitySet::GiveToAbilitySystem(ULRAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles,
	UObject* SourceObject) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp,Error,TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid"),AbilityIndex,*GetNameSafe(this));
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp,Error,TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"),EffectIndex,*GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffecthandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect,EffectToGrant.EffectLevel,ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffecthandle);
		}
	}

	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp,Error,TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"),SetIndex,*GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(),SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);
		
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
