// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNStatics.h"

#include "Experemental/HTN/HTNManager.h"
#include "Experemental/HTN/HTNPlanner.h"
#include "Experemental/HTN/HTNWorldState.h"
#include "Experemental/HTN/Test/QuestTest.h"

UHTNExecutor* UHTNStatics::CreateHTNExecutor(const UObject* WorldContextObject, FName ExecutorID,
                                             EHTNPlannerMode PlannerMode)
{
	UHTNManager* Manager = UHTNManager::GetHTNManager(WorldContextObject);
	if (!Manager) return nullptr;
    
	UHTNExecutor* Executor = Manager->CreateExecutor(ExecutorID);
	if (Executor)
	{
		UHTNPlanner* Planner = Manager->CreatePlanner(PlannerMode);
		Executor->Planner = Planner;
		Executor->WorldState = CreateWorldState();
	}
    
	return Executor;
}

UHTNWorldState* UHTNStatics::CreateWorldState()
{
	return NewObject<UHTNWorldState>(GetTransientPackage());
}

void UHTNStatics::AddWorldProperty(UHTNWorldState* WorldState, FName PropertyName, const FString& StringValue,
	float NumericValue, bool BoolValue)
{
	if (!WorldState) return;
    
	FHTNWorldProperty Property;
	Property.PropertyName = PropertyName;
	Property.Value = StringValue;
	Property.NumericValue = NumericValue;
	Property.BoolValue = BoolValue;
    
	WorldState->SetProperty(PropertyName, Property);
}

UHTNTask* UHTNStatics::CreateTask(TSubclassOf<UHTNTask> TaskClass, FName TaskName,
	const TArray<FHTNCondition>& Preconditions, const TArray<FHTNEffect>& Effects)
{
	if (!TaskClass) return nullptr;
    
	UHTNTask* Task = NewObject<UHTNTask>(GetTransientPackage(), TaskClass);
	Task->TaskName = TaskName;
	Task->Preconditions = Preconditions;
	Task->Effects = Effects;
    
	return Task;
}

FHTNPlan UHTNStatics::PlanHTN(UHTNPlanner* Planner, UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	if (!Planner || !RootTask || !InitialState)
	{
		return FHTNPlan();
	}
    
	return Planner->CreatePlan(RootTask, InitialState);
}

void UHTNStatics::ExecuteHTNPlan(UHTNExecutor* Executor, const FHTNPlan& Plan)
{
	if (!Executor) return;
    
	Executor->ExecutePlan(Plan);
}

UQuestTask* UHTNStatics::CreateQuestTask(FName QuestID, const TArray<FQuestObjective>& Objectives)
{
	UQuestTask* QuestTask = NewObject<UQuestTask>(GetTransientPackage());
	QuestTask->QuestID = QuestID;
	QuestTask->Objectives = Objectives;
	QuestTask->GenerateQuestMethods();
    
	return QuestTask;
}

void UHTNStatics::RegisterAgentToCoordinator(const UObject* WorldContextObject, FName AgentID, UHTNExecutor* Executor,
	const TArray<FName>& Capabilities)
{
	UHTNManager* Manager = UHTNManager::GetHTNManager(WorldContextObject);
	if (!Manager || !Manager->MultiAgentCoordinator) return;
    
	Manager->MultiAgentCoordinator->RegisterAgent(AgentID, Executor, Capabilities);
}

void UHTNStatics::AssignSharedTaskToAgents(const UObject* WorldContextObject, UHTNTask* Task)
{
	UHTNManager* Manager = UHTNManager::GetHTNManager(WorldContextObject);
	if (!Manager || !Manager->MultiAgentCoordinator) return;
    
	Manager->MultiAgentCoordinator->AssignSharedTask(Task);
}

FString UHTNStatics::GetPlanDebugString(const FHTNPlan& Plan)
{
	FString DebugString = FString::Printf(TEXT("HTN Plan (Valid: %s, Steps: %d, Cost: %.2f, Utility: %.2f)\n"),
		Plan.bIsValid ? TEXT("Yes") : TEXT("No"),
		Plan.Steps.Num(),
		Plan.TotalCost,
		Plan.TotalUtility);
    
	for (int32 i = 0; i < Plan.Steps.Num(); i++)
	{
		const FHTNPlanStep& Step = Plan.Steps[i];
		if (Step.Task)
		{
			DebugString += FString::Printf(TEXT("  [%d] %s (Depth: %d, Cost: %.2f, Utility: %.2f)\n"),
				i,
				*Step.Task->TaskName.ToString(),
				Step.Depth,
				Step.EstimatedCost,
				Step.UtilityScore);
		}
	}
    
	return DebugString;
}

void UHTNStatics::DrawHTNPlanVisualization(const UObject* WorldContextObject, const FHTNPlan& Plan,
	FVector BaseLocation, float NodeSpacing)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (!World || !Plan.bIsValid) return;
    
	TMap<int32, FVector> DepthPositions;
	TMap<int32, int32> DepthCounts;
    
	for (const FHTNPlanStep& Step : Plan.Steps)
	{
		if (!DepthCounts.Contains(Step.Depth))
		{
			DepthCounts.Add(Step.Depth, 0);
		}
		DepthCounts[Step.Depth]++;
	}
    
	FVector PreviousLocation = BaseLocation;
    
	for (int32 i = 0; i < Plan.Steps.Num(); i++)
	{
		const FHTNPlanStep& Step = Plan.Steps[i];
        
		FVector NodeLocation = BaseLocation;
		NodeLocation.Z += Step.Depth * NodeSpacing;
		NodeLocation.X += (i % 3) * NodeSpacing;
		NodeLocation.Y += (i / 3) * NodeSpacing;
        
		FColor NodeColor = FColor::Green;
		if (Step.Task)
		{
			if (Step.Task->Status == EHTNTaskStatus::Running)
				NodeColor = FColor::Yellow;
			else if (Step.Task->Status == EHTNTaskStatus::Failed)
				NodeColor = FColor::Red;
			else if (Step.Task->Status == EHTNTaskStatus::Success)
				NodeColor = FColor::Blue;
		}
        
		DrawDebugSphere(World, NodeLocation, 20.0f, 12, NodeColor, false, 5.0f);
        
		if (Step.Task)
		{
			DrawDebugString(World, NodeLocation + FVector(0, 0, 30),
				Step.Task->TaskName.ToString(), nullptr, NodeColor, 5.0f);
		}
        
		if (i > 0)
		{
			DrawDebugLine(World, PreviousLocation, NodeLocation, FColor::White, false, 5.0f);
		}
        
		PreviousLocation = NodeLocation;
	}
}
