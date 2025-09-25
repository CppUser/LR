// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HTNTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HTNStatics.generated.h"

class UHTNPlanner;
class UQuestTask;
struct FQuestObjective;
class UHTNTask;
struct FHTNPlan;
class UHTNExecutor;
/**
 * 
 */
UCLASS()
class LR_API UHTNStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HTN", meta = (WorldContext = "WorldContextObject"))
	static UHTNExecutor* CreateHTNExecutor(
		const UObject* WorldContextObject,
		FName ExecutorID,
		EHTNPlannerMode PlannerMode = EHTNPlannerMode::Dynamic
	);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	static UHTNWorldState* CreateWorldState();

	UFUNCTION(BlueprintCallable, Category = "HTN")
	static void AddWorldProperty(
		UHTNWorldState* WorldState,
		FName PropertyName,
		const FString& StringValue,
		float NumericValue = 0.0f,
		bool BoolValue = false
	);

	UFUNCTION(BlueprintCallable, Category = "HTN", meta = (DeterminesOutputType = "TaskClass"))
	static UHTNTask* CreateTask(
		TSubclassOf<UHTNTask> TaskClass,
		FName TaskName,
		const TArray<FHTNCondition>& Preconditions,
		const TArray<FHTNEffect>& Effects
	);

	// Planning helpers
	UFUNCTION(BlueprintCallable, Category = "HTN")
	static FHTNPlan PlanHTN(
		UHTNPlanner* Planner,
		UHTNTask* RootTask,
		UHTNWorldState* InitialState
	);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	static void ExecuteHTNPlan(
		UHTNExecutor* Executor,
		const FHTNPlan& Plan
	);

	// Quest integration
	UFUNCTION(BlueprintCallable, Category = "HTN|Quest")
	static UQuestTask* CreateQuestTask(
		FName QuestID,
		const TArray<FQuestObjective>& Objectives
	);

	// Multi-agent helpers
	UFUNCTION(BlueprintCallable, Category = "HTN|MultiAgent", meta = (WorldContext = "WorldContextObject"))
	static void RegisterAgentToCoordinator(
		const UObject* WorldContextObject,
		FName AgentID,
		UHTNExecutor* Executor,
		const TArray<FName>& Capabilities
	);

	UFUNCTION(BlueprintCallable, Category = "HTN|MultiAgent", meta = (WorldContext = "WorldContextObject"))
	static void AssignSharedTaskToAgents(
		const UObject* WorldContextObject,
		UHTNTask* Task
	);

	// Debugging helpers
	UFUNCTION(BlueprintCallable, Category = "HTN|Debug")
	static FString GetPlanDebugString(const FHTNPlan& Plan);

	UFUNCTION(BlueprintCallable, Category = "HTN|Debug")
	static void DrawHTNPlanVisualization(
		const UObject* WorldContextObject,
		const FHTNPlan& Plan,
		FVector BaseLocation,
		float NodeSpacing = 100.0f
	);
	
};
