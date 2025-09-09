// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "LRGameplayTagStack.generated.h"

struct FGameplayTagStackContainer;

USTRUCT(BlueprintType)
struct FGameplayTagStack 
{
	GENERATED_BODY()
	friend FGameplayTagStackContainer;
public:
	FGameplayTagStack()
	{}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

	FString GetDebugString() const;

private:
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FGameplayTagStackContainer
{
	GENERATED_BODY()
public:
	FGameplayTagStackContainer()
	{
	}

	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}
private:
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;
	TMap<FGameplayTag, int32> TagToCountMap;
};