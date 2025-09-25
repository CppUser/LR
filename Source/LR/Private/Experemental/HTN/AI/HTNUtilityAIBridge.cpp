// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/AI/HTNUtilityAIBridge.h"

#include "Experemental/HTN/HTNWorldState.h"

float FHTNUtilityConsideration::Evaluate(UHTNWorldState* WorldState) const
{
	if (!WorldState) return 0.0f;
    
	float TotalScore = 1.0f;
    
	for (const FHTNUtilityCurve& UtilityCurve : Curves)
	{
		if (!UtilityCurve.Curve) continue;
        
		FHTNWorldProperty Property = WorldState->GetProperty(UtilityCurve.InputProperty);
		float InputValue = Property.NumericValue;
        
		float CurveValue = UtilityCurve.Curve->GetFloatValue(InputValue);
        
		TotalScore *= FMath::Pow(CurveValue, UtilityCurve.Weight);
	}
    
	TotalScore = FMath::Clamp(TotalScore + Bonus, 0.0f, 1.0f);
    
	return TotalScore;
}

float UHTNUtilitySelector::EvaluateUtility(UHTNTask* Task, UHTNWorldState* WorldState)
{
	if (!Task || !WorldState) return 0.0f;
    
	float TotalUtility = 0.0f;
	float TotalWeight = 0.0f;
    
	for (const FHTNUtilityConsideration& Consideration : Considerations)
	{
		float Score = Consideration.Evaluate(WorldState);
		TotalUtility += Score;
		TotalWeight += 1.0f;
	}
    
	if (TotalWeight > 0.0f)
	{
		TotalUtility /= TotalWeight;
	}
    
	TotalUtility *= Task->UtilityScore;
    
	return TotalUtility;
}

UHTNTask* UHTNUtilitySelector::SelectBestTask(const TArray<UHTNTask*>& Tasks, UHTNWorldState* WorldState)
{
	if (Tasks.Num() == 0 || !WorldState) return nullptr;
    
	UHTNTask* BestTask = nullptr;
	float BestUtility = -1.0f;
    
	for (UHTNTask* Task : Tasks)
	{
		if (!Task) continue;
        
		float Utility = EvaluateUtility(Task, WorldState);
        
		if (Utility > BestUtility)
		{
			BestUtility = Utility;
			BestTask = Task;
		}
	}
    
	return BestTask;
}

TArray<UHTNTask*> UHTNUtilitySelector::RankTasksByUtility(const TArray<UHTNTask*>& Tasks, UHTNWorldState* WorldState)
{
	TArray<UHTNTask*> RankedTasks = Tasks;
    
	if (!WorldState) return RankedTasks;
    
	RankedTasks.Sort([this, WorldState](const UHTNTask& A, const UHTNTask& B)
	{
		float UtilityA = EvaluateUtility(const_cast<UHTNTask*>(&A), WorldState);
		float UtilityB = EvaluateUtility(const_cast<UHTNTask*>(&B), WorldState);
		return UtilityA > UtilityB;
	});
    
	return RankedTasks;
}
