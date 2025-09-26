// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNTask.h"

#include "MassExecutionContext.h"
#include "Experemental/HTN/HTNWorldState.h"

bool UHTNTask::CanExecute_Implementation(UHTNWorldState* WorldState) const
{
	if (!WorldState) return false;

	for (const FHTNCondition& Condition : Preconditions)
	{
		if (!Condition.Evaluate(WorldState))
		{
			return false;
		}
	}
    
	return true;
	
}

void UHTNTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	if (!WorldState) return;
    
	UE_LOG(LogHTN, Log, TEXT("Executing task: %s"), *TaskName.ToString());
    
	Status = EHTNTaskStatus::Running;
    
	for (const FHTNEffect& Effect : Effects)
	{
		WorldState->ApplyEffect(Effect);
	}
}

void UHTNTask::Tick_Implementation(float DeltaTime)
{
	//TODO: implement me (GetTimeManager and call update ?)
}

void UHTNTask::Interrupt_Implementation()
{
	UE_LOG(LogHTN, Log, TEXT("Interrupting task: %s"), *TaskName.ToString());
    
	if (Status == EHTNTaskStatus::Running)
	{
		Status = EHTNTaskStatus::Interrupted;
		OnTaskCompleted.Broadcast(this, Status); //TODO: FIX: if interupted it should be complete
	}
}

float UHTNTask::CalculateUtility_Implementation(UHTNWorldState* WorldState) const
{
	if (!WorldState) return 0.0f;
    
	float Utility = UtilityScore;
    
	//TODO: Adjust utility based on how well preconditions are met
	float PreconditionScore = 1.0f;
	for (const FHTNCondition& Condition : Preconditions)
	{
		if (Condition.Evaluate(WorldState))
		{
			PreconditionScore *= 1.2f;
		}
		else
		{
			PreconditionScore *= 0.8f;
		}
	}
    
	return Utility * PreconditionScore;
}

void UHTNTask::SetStatus(EHTNTaskStatus NewStatus)
{
	EHTNTaskStatus OldStatus = Status;
	Status = NewStatus;
    
	if (OldStatus != NewStatus && 
		(NewStatus == EHTNTaskStatus::Success || 
		 NewStatus == EHTNTaskStatus::Failed || 
		 NewStatus == EHTNTaskStatus::Interrupted))
	{
		OnTaskCompleted.Broadcast(this, Status);
	}
}

////////////////////////////////////////////////
///
////////////////////////////////////////////////
void UHTNTaskPool::InitializePool(TSubclassOf<UHTNTask> TaskClass)
{
	if (!TaskClass) return;
    
	UClass* Class = TaskClass.Get();
    
	if (!AvailableTasks.Contains(Class))
	{
		AvailableTasks.Add(Class, TArray<UHTNTask*>());
		InUseTasks.Add(Class, TArray<UHTNTask*>());
	}
    
	for (int32 i = 0; i < InitialPoolSize; i++)
	{
		UHTNTask* NewTask = NewObject<UHTNTask>(this, TaskClass);
		AvailableTasks[Class].Add(NewTask);
	}
}

UHTNTask* UHTNTaskPool::GetTask(TSubclassOf<UHTNTask> TaskClass)
{
	if (!TaskClass) return nullptr;
    
	UClass* Class = TaskClass.Get();
    
	if (!AvailableTasks.Contains(Class))
	{
		InitializePool(TaskClass);
	}
    
	TArray<UHTNTask*>& Available = AvailableTasks[Class];
	TArray<UHTNTask*>& InUse = InUseTasks[Class];
    
	UHTNTask* Task = nullptr;
    
	if (Available.Num() > 0)
	{
		Task = Available.Pop();
	}
	else if (InUse.Num() < MaxPoolSize)
	{
		Task = NewObject<UHTNTask>(this, TaskClass);
	}
	else
	{
		UE_LOG(LogHTN, Warning, TEXT("Task pool exhausted for %s"), *Class->GetName());
		return nullptr;
	}
    
	if (Task)
	{
		InUse.Add(Task);
        
		Task->Status = EHTNTaskStatus::None;
		Task->Priority = 0;
		Task->UtilityScore = 0.0f;
	}
    
	return Task;
}

void UHTNTaskPool::ReturnTask(UHTNTask* Task)
{
	if (!Task) return;
    
	UClass* Class = Task->GetClass();
    
	if (!InUseTasks.Contains(Class)) return;
    
	TArray<UHTNTask*>& InUse = InUseTasks[Class];
	TArray<UHTNTask*>& Available = AvailableTasks[Class];
    
	if (InUse.Remove(Task) > 0)
	{
		Task->Status = EHTNTaskStatus::None;
		Task->OnTaskCompleted.Clear();
        
		Available.Add(Task);
	}
}

void UHTNTaskPool::ClearPool()
{
	AvailableTasks.Empty();
	InUseTasks.Empty();
}
///////////////////////////////////////////////////
///
//////////////////////////////////////////////////

void UHTNMassEntityTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

	UWorld* World = GetWorld();
	if (!World) 
	{
		SetStatus(EHTNTaskStatus::Failed);
		return;
	}
    
	UMassEntitySubsystem* EntitySubsystem = World->GetSubsystem<UMassEntitySubsystem>();
	if (!EntitySubsystem)
	{
		UE_LOG(LogHTN, Error, TEXT("MassEntitySubsystem not found"));
		SetStatus(EHTNTaskStatus::Failed);
		return;
	}
    
	ProcessEntities(EntitySubsystem);
    
	SetStatus(EHTNTaskStatus::Success);
}

void UHTNMassEntityTask::ProcessEntities(UMassEntitySubsystem* EntitySubsystem)
{
	if (!EntitySubsystem) return;

	//TODO: Fix implementation
	
	// FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();
	// FMassExecutionContext ExecutionContext(EntityManager.AsShared());
 //    
	// EntityQuery.ForEachEntityChunk(EntityManager, ExecutionContext, 
	// 	[this, &WorldState = this->WorldState](FMassExecutionContext& Context)
	// {
	// 	const int32 NumEntities = Context.GetNumEntities();
 //        
	// 	for (int32 i = 0; i < NumEntities && i < MaxEntitiesToProcess; i++)
	// 	{
	// 		FHTNWorldProperty EntityProperty;
	// 		EntityProperty.PropertyName = FName(*FString::Printf(TEXT("Entity_%d_Processed"), i));
	// 		EntityProperty.BoolValue = true;
	// 		WorldState->SetProperty(EntityProperty.PropertyName, EntityProperty);
	// 	}
	// });
 //    
	// UE_LOG(LogHTN, Log, TEXT("Processed Mass entities"));
}