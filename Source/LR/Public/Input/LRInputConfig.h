// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "Engine/DataAsset.h"
#include "LRInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FLRInputMapping
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* Context = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct FLRInputAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};


UCLASS(BlueprintType, Const)
class LR_API ULRInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	const UInputAction* FindNativeInputWithTag(const FGameplayTag& InTag) const;
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, meta = (TitleProperty = "Context"))
	FLRInputMapping Mappings;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FLRInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FLRInputAction> AbilityInputActions;
};
