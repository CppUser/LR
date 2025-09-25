// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNExecutor.h"

#include "Experemental/HTN/HTNWorldState.h"

void UHTNExecutor::ExecutePlan(const FHTNPlan& Plan)
{
	if (!Plan.bIsValid)
	{
		UE_LOG(LogHTN, Warning, TEXT("Cannot execute invalid plan"));
		return;
	}
    
	Stop();
    
	CurrentPlan = Plan;
	CurrentStepIndex = 0;
    
	UE_LOG(LogHTN, Log, TEXT("Starting execution of plan with %d steps"), Plan.Steps.Num());
    
	OnPlanStarted.Broadcast(CurrentPlan);
    
	ExecuteNextStep();
}

void UHTNExecutor::Tick(float DeltaTime)
{
	TimeSinceLastReplanCheck += DeltaTime;
	if (TimeSinceLastReplanCheck >= ReplanCheckInterval)
	{
		CheckForInterruptions();
		TimeSinceLastReplanCheck = 0.0f;
	}
    
	if (CurrentTask && CurrentTask->Status == EHTNTaskStatus::Running)
	{
		CurrentTask->Tick(DeltaTime);
	}
}

void UHTNExecutor::InterruptCurrent(UHTNTask* NewHighPriorityTask)
{
	UE_LOG(LogHTN, Log, TEXT("Interrupting current execution"));
    
	if (CurrentTask && CurrentTask->Status == EHTNTaskStatus::Running)
	{
		CurrentTask->Interrupt();
	}
    
	if (NewHighPriorityTask && Planner && WorldState)
	{
		FHTNPlan NewPlan = Planner->CreatePlan(NewHighPriorityTask, WorldState);
        
		if (NewPlan.bIsValid)
		{
			OnPlanInterrupted.Broadcast(CurrentPlan);
			ExecutePlan(NewPlan);
		}
	}
}

void UHTNExecutor::Stop()
{
	if (CurrentTask && CurrentTask->Status == EHTNTaskStatus::Running)
	{
		CurrentTask->Interrupt();
	}
    
	CurrentTask = nullptr;
	CurrentStepIndex = 0;
	CurrentPlan.Clear();
}

void UHTNExecutor::ExecuteNextStep()
{
	if (!CurrentPlan.bIsValid || CurrentStepIndex >= CurrentPlan.Steps.Num())
	{
		OnPlanCompleted.Broadcast(CurrentPlan);
		CurrentPlan.Clear();
		return;
	}
    
	const FHTNPlanStep& Step = CurrentPlan.Steps[CurrentStepIndex];
	CurrentTask = Step.Task;
    
	if (!CurrentTask)
	{
		UE_LOG(LogHTN, Error, TEXT("Invalid task in plan step %d"), CurrentStepIndex);
		CurrentStepIndex++;
		ExecuteNextStep();
		return;
	}
	
	//TODO:CurrentTask->OnTaskCompleted.AddUObject(this, &UHTNExecutor::HandleTaskCompletion);
    
	UE_LOG(LogHTN, Log, TEXT("Executing step %d: %s"), 
		   CurrentStepIndex, *CurrentTask->TaskName.ToString());
    
	CurrentTask->Execute(WorldState);
}

void UHTNExecutor::CheckForInterruptions()
{
	if (!bAllowInterruptions || !CurrentTask || !Planner || !WorldState) return;
    
	static int32 LastKnownStateVersion = 0;
    
	if (WorldState->StateVersion != LastKnownStateVersion)
	{
		LastKnownStateVersion = WorldState->StateVersion;
        
		if (!Planner->ValidatePlan(CurrentPlan, WorldState))
		{
			UE_LOG(LogHTN, Log, TEXT("Current plan invalidated, replanning..."));
            
			FHTNPlan NewPlan = Planner->ReplanFromStep(CurrentPlan, CurrentStepIndex, WorldState);
            
			if (NewPlan.bIsValid && NewPlan.TotalUtility > CurrentPlan.TotalUtility * 1.2f)
			{
				if (CurrentTask && CurrentTask->Status == EHTNTaskStatus::Running)
				{
					CurrentTask->Interrupt();
				}
                
				CurrentPlan = NewPlan;
				CurrentStepIndex = 0;
				OnPlanInterrupted.Broadcast(CurrentPlan);
				ExecuteNextStep();
			}
		}
	}
}

void UHTNExecutor::HandleTaskCompletion(UHTNTask* Task, EHTNTaskStatus Status)
{
	if (Task != CurrentTask) return;
    
	UE_LOG(LogHTN, Log, TEXT("Task %s completed with status: %d"), 
		   *Task->TaskName.ToString(), (int32)Status);
    
	Task->OnTaskCompleted.RemoveAll(this);
    
	switch (Status)
	{
	case EHTNTaskStatus::Success:
		CurrentStepIndex++;
		ExecuteNextStep();
		break;
        
	case EHTNTaskStatus::Failed:
		if (Planner && WorldState)
		{
			FHTNPlan NewPlan = Planner->ReplanFromStep(CurrentPlan, CurrentStepIndex, WorldState);
			if (NewPlan.bIsValid)
			{
				CurrentPlan = NewPlan;
				CurrentStepIndex = 0;
				ExecuteNextStep();
			}
			else
			{
				UE_LOG(LogHTN, Error, TEXT("Task failed and replanning failed"));
				Stop();
			}
		}
		break;
        
	case EHTNTaskStatus::Interrupted:
		break;
        
	default:
		break;
	}
}
