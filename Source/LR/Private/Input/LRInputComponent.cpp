// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Input/LRInputComponent.h"

void ULRInputComponent::AddInputMappings(const ULRInputConfig* InInputConfig,
										 UEnhancedInputLocalPlayerSubsystem* InInputSubsystem) const
{
	check(InInputConfig);
	check(InInputSubsystem);

	//TODO: Should be handled by Input Settings
	InInputSubsystem->AddMappingContext(InInputConfig->Mappings.Context,InInputConfig->Mappings.Priority);

}

void ULRInputComponent::RemoveInputMappings(const ULRInputConfig* InInputConfig,
	UEnhancedInputLocalPlayerSubsystem* InInputSubsystem)
{
	check(InInputConfig);
	check(InInputSubsystem);

	//TODO: Should be handled by Input Settings
	InInputSubsystem->RemoveMappingContext(InInputConfig->Mappings.Context);
}

void ULRInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}


