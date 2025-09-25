// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Experemental/HTN/HTNTask.h"
#include "HTNUtilityAIBridge.generated.h"

USTRUCT(BlueprintType)
struct FHTNUtilityCurve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* Curve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InputProperty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1.0f;
};

USTRUCT(BlueprintType)
struct FHTNUtilityConsideration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ConsiderationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNUtilityCurve> Curves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Bonus = 0.0f;

	float Evaluate(UHTNWorldState* WorldState) const;
};

UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNUtilitySelector : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utility")
	float EvaluateUtility(UHTNTask* Task, UHTNWorldState* WorldState);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	UHTNTask* SelectBestTask(const TArray<UHTNTask*>& Tasks, UHTNWorldState* WorldState);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	TArray<UHTNTask*> RankTasksByUtility(const TArray<UHTNTask*>& Tasks, UHTNWorldState* WorldState);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
	TArray<FHTNUtilityConsideration> Considerations;

};
