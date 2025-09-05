// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Input/LRInputConfig.h"

const UInputAction* ULRInputConfig::FindNativeInputWithTag(const FGameplayTag& InTag) const
{
	for (const FLRInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}
