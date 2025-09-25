// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNPlanner.h"

#include "Experemental/HTN/HTNCompoundTask.h"
#include "Experemental/HTN/HTNWorldState.h"

FHTNPlan UHTNPlanner::CreatePlan(UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	FHTNPlan Plan;
	Plan.bIsValid = false;
    
	if (!RootTask || !InitialState)
	{
		UE_LOG(LogHTN, Error, TEXT("CreatePlan: Invalid input"));
		return Plan;
	}
    
	UHTNWorldState* WorkingState = InitialState->Clone();
    
	FDateTime StartTime = FDateTime::Now();
	bool bSuccess = DecomposeTask(RootTask, WorkingState, Plan, 0);
    
	float ElapsedSeconds = (FDateTime::Now() - StartTime).GetTotalSeconds();
	if (ElapsedSeconds > PlanTimeout)
	{
		UE_LOG(LogHTN, Warning, TEXT("Planning timeout exceeded"));
		if (!bAllowPartialPlans)
		{
			Plan.Clear();
			return Plan;
		}
	}
    
	if (bSuccess || (bAllowPartialPlans && Plan.Steps.Num() > 0))
	{
		Plan.bIsValid = true;
		Plan.TotalUtility = EvaluatePlanUtility(Plan, InitialState);
	}
    
	UE_LOG(LogHTN, Log, TEXT("Created plan with %d steps, utility: %f"), 
		   Plan.Steps.Num(), Plan.TotalUtility);
    
	return Plan;
}

FHTNPlan UHTNPlanner::ReplanFromStep(const FHTNPlan& CurrentPlan, int32 StepIndex, UHTNWorldState* CurrentState)
{
	FHTNPlan NewPlan;
	NewPlan.bIsValid = false;
    
	if (!CurrentState || StepIndex >= CurrentPlan.Steps.Num())
	{
		return NewPlan;
	}
    
	for (int32 i = 0; i < StepIndex; i++)
	{
		NewPlan.Steps.Add(CurrentPlan.Steps[i]);
		NewPlan.TotalCost += CurrentPlan.Steps[i].EstimatedCost;
	}
    
	if (StepIndex < CurrentPlan.Steps.Num())
	{
		UHTNTask* RemainingTask = CurrentPlan.Steps[StepIndex].Task;
		if (RemainingTask)
		{
			UHTNWorldState* WorkingState = CurrentState->Clone();
			bool bSuccess = DecomposeTask(RemainingTask, WorkingState, NewPlan, StepIndex);
            
			if (bSuccess || (bAllowPartialPlans && NewPlan.Steps.Num() > StepIndex))
			{
				NewPlan.bIsValid = true;
				NewPlan.TotalUtility = EvaluatePlanUtility(NewPlan, CurrentState);
			}
		}
	}
    
	return NewPlan;
}

bool UHTNPlanner::ValidatePlan(const FHTNPlan& Plan, UHTNWorldState* CurrentState) const
{
	if (!Plan.bIsValid || !CurrentState) return false;
    
	UHTNWorldState* SimulatedState = CurrentState->Clone();
    
	for (const FHTNPlanStep& Step : Plan.Steps)
	{
		if (!Step.Task) return false;
        
		if (!Step.Task->CanExecute(SimulatedState))
		{
			return false;
		}
        
		for (const FHTNEffect& Effect : Step.Task->Effects)
		{
			SimulatedState->ApplyEffect(Effect);
		}
	}
    
	return true;
}

bool UHTNPlanner::DecomposeTask(UHTNTask* Task, UHTNWorldState* WorldState, FHTNPlan& OutPlan, int32 CurrentDepth)
{
	if (!Task || !WorldState) return false;
    
    if (CurrentDepth > MaxPlanDepth)
    {
        UE_LOG(LogHTN, Warning, TEXT("Max plan depth reached"));
        return false;
    }
    
    if (!Task->CanExecute(WorldState))
    {
        UE_LOG(LogHTN, Verbose, TEXT("Task %s cannot execute"), *Task->TaskName.ToString());
        return false;
    }
    
    if (UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(Task))
    {
        TArray<FHTNMethod> ValidMethods = CompoundTask->GetValidMethods(WorldState);
        
        if (ValidMethods.Num() == 0)
        {
            UE_LOG(LogHTN, Verbose, TEXT("No valid methods for compound task %s"), 
                   *Task->TaskName.ToString());
            return false;
        }
        
        FHTNMethod SelectedMethod;
        
        switch (PlannerMode)
        {
        case EHTNPlannerMode::Dynamic:
        case EHTNPlannerMode::UtilityDriven:
            SelectedMethod = CompoundTask->SelectBestMethod(ValidMethods, WorldState);
            break;
            
        case EHTNPlannerMode::Probabilistic:
            {
                // Probabilistic selection
                float TotalScore = 0.0f;
                for (const FHTNMethod& Method : ValidMethods)
                {
                    TotalScore += Method.PreferenceScore;
                }
                
                float Random = FMath::FRandRange(0.0f, TotalScore);
                float Current = 0.0f;
                
                for (const FHTNMethod& Method : ValidMethods)
                {
                    Current += Method.PreferenceScore;
                    if (Random <= Current)
                    {
                        SelectedMethod = Method;
                        break;
                    }
                }
            }
            break;
            
        default:
            SelectedMethod = ValidMethods[0];
            break;
        }
        
        UHTNWorldState* TempWorldState = WorldState->Clone();
        bool bAllSubtasksSucceeded = true;
        
        for (TSubclassOf<UHTNTask> SubtaskClass : SelectedMethod.Subtasks)
        {
            if (!SubtaskClass) continue;
            
            UHTNTask* Subtask = NewObject<UHTNTask>(this, SubtaskClass);
            
            if (!DecomposeTask(Subtask, TempWorldState, OutPlan, CurrentDepth + 1))
            {
                bAllSubtasksSucceeded = false;
                if (!bAllowPartialPlans) return false;
            }
            
            for (const FHTNEffect& Effect : Subtask->Effects)
            {
                TempWorldState->ApplyEffect(Effect);
            }
        }
        
        return bAllSubtasksSucceeded || (bAllowPartialPlans && OutPlan.Steps.Num() > 0);
    }
    else // Primitive task
    {
        FHTNPlanStep Step;
        Step.Task = Task;
        Step.Depth = CurrentDepth;
        Step.EstimatedCost = 1.0f;
        Step.UtilityScore = Task->CalculateUtility(WorldState);
        
        OutPlan.Steps.Add(Step);
        OutPlan.TotalCost += Step.EstimatedCost;
        OutPlan.TotalUtility += Step.UtilityScore;
        
        return true;
    }
}

float UHTNPlanner::EvaluatePlanUtility(const FHTNPlan& Plan, UHTNWorldState* WorldState) const
{
	float TotalUtility = 0.0f;
	float DiscountFactor = 1.0f;
    
	UHTNWorldState* SimulatedState = WorldState->Clone();
    
	for (const FHTNPlanStep& Step : Plan.Steps)
	{
		if (!Step.Task) continue;
        
		float StepUtility = Step.Task->CalculateUtility(SimulatedState);
		TotalUtility += StepUtility * DiscountFactor;
        
		DiscountFactor *= 0.95f;
        
		for (const FHTNEffect& Effect : Step.Task->Effects)
		{
			SimulatedState->ApplyEffect(Effect);
		}
	}
    
	return TotalUtility;
}

/////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
void UHTNAsyncPlanner::CreatePlanAsync(UHTNTask* RootTask, UHTNWorldState* InitialState)
{
}

bool UHTNAsyncPlanner::IsPlanningInProgress() const
{
	//TODO: implement me
	return false;
}

void UHTNAsyncPlanner::CancelAsyncPlanning()
{
}

void UHTNAsyncPlanner::OnPlanningComplete()
{
}
