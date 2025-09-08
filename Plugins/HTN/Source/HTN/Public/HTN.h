// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HTN.generated.h"

UCLASS(BlueprintType)
class HTN_API UHTN : public UObject
{
	GENERATED_BODY()
public:
	bool ValidateAsset(TArray<FString>& Errors);
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* Graph;
#endif

	UPROPERTY()
	class UBlackboardData* BlackboardAsset;
};