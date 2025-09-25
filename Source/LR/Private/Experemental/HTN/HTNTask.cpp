// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNTask.h"

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
}

UHTNTask* UHTNTaskPool::GetTask(TSubclassOf<UHTNTask> TaskClass)
{
	//TODO: implement me
	return nullptr;
}

void UHTNTaskPool::ReturnTask(UHTNTask* Task)
{
}

void UHTNTaskPool::ClearPool()
{
}
///////////////////////////////////////////////////
///
//////////////////////////////////////////////////

void UHTNMassEntityTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);
}

void UHTNMassEntityTask::ProcessEntities(UMassEntitySubsystem* EntitySubsystem)
{
}