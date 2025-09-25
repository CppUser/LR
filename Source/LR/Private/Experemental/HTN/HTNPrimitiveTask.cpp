// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNPrimitiveTask.h"

void UHTNPrimitiveTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

	CurrentExecutionTime = 0.0f;

	OnExecuteAction(WorldState);

	if (ExecutionTime <= 0.0f)
	{
		SetStatus(EHTNTaskStatus::Success);
	}
}

void UHTNPrimitiveTask::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	CurrentExecutionTime += DeltaTime;

	if (ExecutionTime > 0.0f && CurrentExecutionTime >= ExecutionTime)
	{
		//TODO: Using probability to determine success/failure
		float Random = FMath::FRand();
		if (Random <= SuccessProbability)
		{
			SetStatus(EHTNTaskStatus::Success);
		}
		else
		{
			SetStatus(EHTNTaskStatus::Failed);
		}
	}
}

void UHTNPrimitiveTask::Interrupt_Implementation()
{
	if (!bIsInterruptible && Status == EHTNTaskStatus::Running)
	{
		UE_LOG(LogHTN, Warning, TEXT("Task %s cannot be interrupted"), *TaskName.ToString());
		return;
	}
    
	Super::Interrupt_Implementation();
	CurrentExecutionTime = 0.0f;
}



void UHTNPrimitiveTask::OnExecuteAction_Implementation(UHTNWorldState* WorldState)
{
}

///////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////
void UHTNSmartObjectTask::OnExecuteAction_Implementation(UHTNWorldState* WorldState)
{
	Super::OnExecuteAction_Implementation(WorldState);
}