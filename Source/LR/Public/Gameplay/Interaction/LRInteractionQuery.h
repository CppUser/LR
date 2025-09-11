﻿#pragma once

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