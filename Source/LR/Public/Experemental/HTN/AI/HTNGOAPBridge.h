// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Experemental/HTN/HTNTask.h"
#include "UObject/Object.h"
#include "HTNGOAPBridge.generated.h"

USTRUCT(BlueprintType)
struct FGOAPAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNCondition> Preconditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNEffect> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cost = 1.0f;
};

USTRUCT(BlueprintType)
struct FGOAPGoal
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GoalName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNCondition> DesiredState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Priority = 1.0f;
};


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNGOAPBridge : public UHTNTask
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	TArray<FGOAPAction> PlanGOAP(UHTNWorldState* CurrentState);

	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	FGOAPGoal Goal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	TArray<FGOAPAction> AvailableActions;

protected:
	TArray<FGOAPAction> CurrentGOAPPlan;
	int32 CurrentActionIndex = 0;
};
