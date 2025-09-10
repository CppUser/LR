// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "LRInteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FInteractionQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingAvatar;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};