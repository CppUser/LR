// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNCompoundTask.h"

TArray<FHTNMethod> UHTNCompoundTask::GetValidMethods(UHTNWorldState* WorldState) const
{
	TArray<FHTNMethod> ValidMethods;
    
	for (const FHTNMethod& Method : Methods)
	{
		bool bIsValid = true;
        
		for (const FHTNCondition& Condition : Method.Preconditions)
		{
			if (!Condition.Evaluate(WorldState))
			{
				bIsValid = false;
				break;
			}
		}
        
		if (bIsValid)
		{
			ValidMethods.Add(Method);
		}
	}
    
	return ValidMethods;
}

FHTNMethod UHTNCompoundTask::SelectBestMethod_Implementation(const TArray<FHTNMethod>& ValidMethods,
	UHTNWorldState* WorldState) const
{
	//TODO: This function simply prototype , research more
	
	if (ValidMethods.Num() == 0) return FHTNMethod();
    
	FHTNMethod BestMethod = ValidMethods[0];
	float BestScore = 0.0f;
    
	for (const FHTNMethod& Method : ValidMethods)
	{
		float Score = Method.PreferenceScore;
        
		for (const TSubclassOf<UHTNTask>& SubtaskClass : Method.Subtasks)
		{
			if (!SubtaskClass) continue;
            
			UHTNTask* TempTask = NewObject<UHTNTask>(GetTransientPackage(), SubtaskClass);
			Score += TempTask->CalculateUtility(WorldState);
		}
        
		if (Score > BestScore)
		{
			BestScore = Score;
			BestMethod = Method;
		}
	}
    
	return BestMethod;
}

void UHTNCompoundTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

	// Compound tasks don't execute directly - they decompose
	// This should be handled by the planner
	SetStatus(EHTNTaskStatus::Success);
}
