// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNStatics.h"

#include "Experemental/HTN/HTNPlanner.h"

UHTNExecutor* UHTNStatics::CreateHTNExecutor(const UObject* WorldContextObject, FName ExecutorID,
                                             EHTNPlannerMode PlannerMode)
{
	return nullptr;
}

UHTNWorldState* UHTNStatics::CreateWorldState()
{
	return nullptr;
}

void UHTNStatics::AddWorldProperty(UHTNWorldState* WorldState, FName PropertyName, const FString& StringValue,
	float NumericValue, bool BoolValue)
{
}

UHTNTask* UHTNStatics::CreateTask(TSubclassOf<UHTNTask> TaskClass, FName TaskName,
	const TArray<FHTNCondition>& Preconditions, const TArray<FHTNEffect>& Effects)
{
	return nullptr;
}

FHTNPlan UHTNStatics::PlanHTN(UHTNPlanner* Planner, UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	return FHTNPlan();
}

void UHTNStatics::ExecuteHTNPlan(UHTNExecutor* Executor, const FHTNPlan& Plan)
{
}

UQuestTask* UHTNStatics::CreateQuestTask(FName QuestID, const TArray<FQuestObjective>& Objectives)
{
	return nullptr;
}

void UHTNStatics::RegisterAgentToCoordinator(const UObject* WorldContextObject, FName AgentID, UHTNExecutor* Executor,
	const TArray<FName>& Capabilities)
{
}

void UHTNStatics::AssignSharedTaskToAgents(const UObject* WorldContextObject, UHTNTask* Task)
{
}

FString UHTNStatics::GetPlanDebugString(const FHTNPlan& Plan)
{
	return "";
}

void UHTNStatics::DrawHTNPlanVisualization(const UObject* WorldContextObject, const FHTNPlan& Plan,
	FVector BaseLocation, float NodeSpacing)
{
}
