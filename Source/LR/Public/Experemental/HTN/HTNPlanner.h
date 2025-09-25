// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "UObject/Object.h"
#include "HTNPlanner.generated.h"


USTRUCT(BlueprintType)
struct FHTNPlanStep
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UHTNTask* Task = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 Depth = 0;

	UPROPERTY(BlueprintReadOnly)
	float EstimatedCost = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float UtilityScore = 0.0f;
};

USTRUCT(BlueprintType)
struct FHTNPlan
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FHTNPlanStep> Steps;

	UPROPERTY(BlueprintReadOnly)
	float TotalCost = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float TotalUtility = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid = false;

	void Clear()
	{
		Steps.Empty();
		TotalCost = 0.0f;
		TotalUtility = 0.0f;
		bIsValid = false;
	}
};


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNPlanner : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HTN")
	FHTNPlan CreatePlan(UHTNTask* RootTask, UHTNWorldState* InitialState);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	FHTNPlan ReplanFromStep(const FHTNPlan& CurrentPlan, int32 StepIndex, UHTNWorldState* CurrentState);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	bool ValidatePlan(const FHTNPlan& Plan, UHTNWorldState* CurrentState) const;

protected:
	bool DecomposeTask(UHTNTask* Task, UHTNWorldState* WorldState, FHTNPlan& OutPlan, int32 CurrentDepth);
	float EvaluatePlanUtility(const FHTNPlan& Plan, UHTNWorldState* WorldState) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	EHTNPlannerMode PlannerMode = EHTNPlannerMode::Dynamic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	int32 MaxPlanDepth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	float PlanTimeout = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	bool bAllowPartialPlans = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	bool bUseAsyncPlanning = false;
};


class FHTNPlanningTask : public FNonAbandonableTask
{
public:
	FHTNPlanningTask(
		UHTNTask* InRootTask,
		UHTNWorldState* InWorldState,
		UHTNPlanner* InPlanner
	);

	void DoWork();
    
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FHTNPlanningTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	FHTNPlan ResultPlan;

private:
	UHTNTask* RootTask;
	UHTNWorldState* WorldState;
	UHTNPlanner* Planner;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHTNAsyncPlanComplete, const FHTNPlan&, Plan);

UCLASS(BlueprintType)
class LR_API UHTNAsyncPlanner : public UHTNPlanner
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "HTN")
	void CreatePlanAsync(UHTNTask* RootTask, UHTNWorldState* InitialState);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	bool IsPlanningInProgress() const;

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void CancelAsyncPlanning();

protected:
	void OnPlanningComplete();
public:
	UPROPERTY(BlueprintAssignable)
	FHTNAsyncPlanComplete OnAsyncPlanComplete;
protected:
	TUniquePtr<FAsyncTask<FHTNPlanningTask>> CurrentPlanningTask;
    FTimerHandle PlanningCheckTimer;
};