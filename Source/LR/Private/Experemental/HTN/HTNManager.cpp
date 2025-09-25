// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNManager.h"

#include "Experemental/HTN/HTNWorldState.h"

void UHTNManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GlobalWorldState = NewObject<UHTNWorldState>(this);
    
	MultiAgentCoordinator = NewObject<UHTNMultiAgentCoordinator>(this);
	MultiAgentCoordinator->SharedWorldState = GlobalWorldState;
    
	UtilitySelector = NewObject<UHTNUtilitySelector>(this);
    
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TickTimerHandle,
			[this]()
			{
				float DeltaTime = 0.1f; // TODO: compute from World->GetDeltaSeconds() ???
				TickHTNSystem(DeltaTime);
			},
			0.1f,
			true
		);
	}
    
	UE_LOG(LogHTN, Log, TEXT("HTN Manager initialized"));
}

void UHTNManager::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TickTimerHandle);
	}
    
	for (auto& ExecutorPair : ActiveExecutors)
	{
		if (ExecutorPair.Value)
		{
			ExecutorPair.Value->Stop();
		}
	}
	ActiveExecutors.Empty();
	
	Super::Deinitialize();
}

UHTNExecutor* UHTNManager::CreateExecutor(FName ExecutorID)
{
	if (ActiveExecutors.Contains(ExecutorID))
	{
		UE_LOG(LogHTN, Warning, TEXT("Executor %s already exists"), *ExecutorID.ToString());
		return ActiveExecutors[ExecutorID];
	}
    
	UHTNExecutor* NewExecutor = NewObject<UHTNExecutor>(this);
	ActiveExecutors.Add(ExecutorID, NewExecutor);
    
	UE_LOG(LogHTN, Log, TEXT("Created executor: %s"), *ExecutorID.ToString());
    
	return NewExecutor;
}

void UHTNManager::DestroyExecutor(FName ExecutorID)
{
	UHTNExecutor** FoundExecutor = ActiveExecutors.Find(ExecutorID);
	if (FoundExecutor && *FoundExecutor)
	{
		(*FoundExecutor)->Stop();
		ActiveExecutors.Remove(ExecutorID);
        
		UE_LOG(LogHTN, Log, TEXT("Destroyed executor: %s"), *ExecutorID.ToString());
	}
}

UHTNPlanner* UHTNManager::CreatePlanner(EHTNPlannerMode Mode)
{
	UHTNPlanner* NewPlanner = NewObject<UHTNPlanner>(this);
	NewPlanner->PlannerMode = Mode;
    
	return NewPlanner;
}

void UHTNManager::TickHTNSystem(float DeltaTime)
{
	for (auto& ExecutorPair : ActiveExecutors)
	{
		if (ExecutorPair.Value)
		{
			ExecutorPair.Value->Tick(DeltaTime);
		}
	}
    
	if (MultiAgentCoordinator)
	{
		MultiAgentCoordinator->Tick(DeltaTime);
	}
}

void UHTNManager::RegisterTaskClass(TSubclassOf<UHTNTask> TaskClass, FName TaskID)
{
	if (!TaskClass) return;
    
	RegisteredTasks.Add(TaskID, TaskClass);
    
	UE_LOG(LogHTN, Log, TEXT("Registered task class: %s"), *TaskID.ToString());
}

UHTNManager* UHTNManager::GetHTNManager(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
    
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
    
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;
    
	return GameInstance->GetSubsystem<UHTNManager>();
}
