// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "UObject/Object.h"
#include "HTNPlanner.generated.h"


class UHTNCompoundTask;
class UHTNExecutor;

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


/////////////////////////////////////////////////////////////////////////////
///                        Reactive Planning
/////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FHTNReactiveRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RuleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNCondition> TriggerConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UHTNTask> ReactiveTask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Priority = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInterruptCurrent = true;
};


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNReactiveLayer : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Reactive")
	void CheckReactiveRules(UHTNWorldState* WorldState);

	UFUNCTION(BlueprintCallable, Category = "Reactive")
	void AddReactiveRule(const FHTNReactiveRule& Rule);

	UFUNCTION(BlueprintCallable, Category = "Reactive")
	void RemoveReactiveRule(FName RuleName);

	void Tick(float DeltaTime);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive")
	TArray<FHTNReactiveRule> ReactiveRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive")
	float ReactiveCheckInterval = 0.1f;

	UPROPERTY(BlueprintReadWrite, Category = "Reactive")
	UHTNExecutor* LinkedExecutor = nullptr;
protected:
	float TimeSinceLastCheck = 0.0f;
	FName LastTriggeredRule;
};

/////////////////////////////////////////////////////////////////////////////
///                      Partial planning
/////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FHTNOrderingConstraint
{
	GENERATED_BODY()

	UPROPERTY()
	int32 BeforeTaskIndex = -1;

	UPROPERTY()
	int32 AfterTaskIndex = -1;

	UPROPERTY()
	bool bStrict = true;
};

USTRUCT(BlueprintType)
struct FHTNPartialPlan
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UHTNTask*> Tasks;

	UPROPERTY()
	TArray<FHTNOrderingConstraint> OrderingConstraints;

	UPROPERTY()
	TMap<int32, int32> Dependencies;
	//TMap<int32, TArray<int32>> Dependencies;

	bool IsValid() const;
	TArray<int32> GetExecutableTaskIndices() const;
};

UCLASS(BlueprintType)
class LR_API UHTNPartialOrderPlanner : public UHTNPlanner
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTN")
	FHTNPartialPlan CreatePartialOrderPlan(UHTNTask* RootTask, UHTNWorldState* InitialState);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	TArray<UHTNTask*> GetParallelExecutableTasks(const FHTNPartialPlan& Plan);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void UpdatePlanConstraints(FHTNPartialPlan& Plan, int32 CompletedTaskIndex);

protected:
	void AnalyzeTaskDependencies(const TArray<UHTNTask*>& Tasks, FHTNPartialPlan& OutPlan);
	bool CanExecuteInParallel(UHTNTask* Task1, UHTNTask* Task2);
};

/////////////////////////////////////////////////////////////////////////////
///            Probabilistic  Planning  - Handle uncertainty
/////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FHTNProbabilisticOutcome
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNEffect> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Probability = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UtilityModifier = 0.0f;
};

UCLASS(BlueprintType)
class LR_API UHTNProbabilisticTask : public UHTNTask
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Probabilistic")
	FHTNProbabilisticOutcome SampleOutcome() const;

	UFUNCTION(BlueprintCallable, Category = "Probabilistic")
	float CalculateExpectedUtility(UHTNWorldState* WorldState) const;

	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Probabilistic")
	TArray<FHTNProbabilisticOutcome> PossibleOutcomes;

};

UCLASS(BlueprintType)
class LR_API UHTNMonteCarloPlanner : public UHTNPlanner
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "MonteCarlo")
	FHTNPlan CreateMonteCarloP (UHTNTask* RootTask, UHTNWorldState* InitialState);

protected:
	float SimulatePlan(const FHTNPlan& Plan, UHTNWorldState* InitialState);
	FHTNPlan SelectBestPlanFromSimulations(const TArray<FHTNPlan>& CandidatePlans);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MonteCarlo")
	int32 NumSimulations = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MonteCarlo")
	float DiscountFactor = 0.95f;

};

/////////////////////////////////////////////////////////////////////////////
///       Emergent Behavior - Emergent behavior through task combination
/////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FHTNBehaviorPattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PatternName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredTasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNCondition> ActivationConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EmergenceThreshold = 0.7f;
};

UCLASS(BlueprintType)
class LR_API UHTNEmergentBehaviorSystem : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Emergent")
	void ObserveTaskSequence(const TArray<UHTNTask*>& ExecutedTasks, bool bSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Emergent")
	TArray<FHTNBehaviorPattern> DetectEmergentPatterns(const TArray<UHTNTask*>& TaskHistory);

	UFUNCTION(BlueprintCallable, Category = "Emergent")
	UHTNCompoundTask* GenerateEmergentTask(const FHTNBehaviorPattern& Pattern);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergent")
	TArray<FHTNBehaviorPattern> KnownPatterns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergent")
	float PatternLearningRate = 0.1f;

protected:
	TMap<FString, float> PatternSuccessRates;
	TArray<TArray<FName>> ObservedSequences;
};

