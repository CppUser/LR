// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNPlanner.h"
#include "UObject/Object.h"
#include "HTNExecutor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHTNPlanEvent, const FHTNPlan&, Plan);


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNExecutor : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HTN")
	void ExecutePlan(const FHTNPlan& Plan);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void InterruptCurrent(UHTNTask* NewHighPriorityTask = nullptr);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void Stop();

protected:
	void ExecuteNextStep();
	void CheckForInterruptions();
	void HandleTaskCompletion(UHTNTask* Task, EHTNTaskStatus Status);

public:
	UPROPERTY(BlueprintReadOnly, Category = "HTN")
	FHTNPlan CurrentPlan;

	UPROPERTY(BlueprintReadOnly, Category = "HTN")
	int32 CurrentStepIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "HTN")
	UHTNTask* CurrentTask = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	UHTNWorldState* WorldState = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	UHTNPlanner* Planner = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	bool bAllowInterruptions = true;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	float ReplanCheckInterval = 0.5f;

	UPROPERTY(BlueprintAssignable)
	FHTNPlanEvent OnPlanStarted;

	UPROPERTY(BlueprintAssignable)
	FHTNPlanEvent OnPlanCompleted;

	UPROPERTY(BlueprintAssignable)
	FHTNPlanEvent OnPlanInterrupted;
protected:
	float TimeSinceLastReplanCheck = 0.0f;
};
