// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HTNTypes.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHTN, Log, All);

UENUM(BlueprintType)
enum class EHTNTaskStatus : uint8
{
	None,
	Running,
	Success,
	Failed,
	Interrupted
};

UENUM(BlueprintType)
enum class EHTNOperatorType : uint8
{
	Add,
	Remove,
	Set
};

UENUM(BlueprintType)
enum class EHTNPlannerMode : uint8
{
	Static,
	Dynamic,
	Probabilistic,
	UtilityDriven
};

USTRUCT(BlueprintType)
struct FHTNWorldProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NumericValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BoolValue = false;
};


USTRUCT(BlueprintType)
struct FHTNEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHTNOperatorType Operator = EHTNOperatorType::Set;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHTNWorldProperty NewValue;
};

USTRUCT(BlueprintType)
struct FHTNCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ExpectedValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CheckNumeric = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NumericThreshold = 0.0f;

	bool Evaluate(class UHTNWorldState* WorldState) const;
};

