// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNTypes.h"

#include "Experemental/HTN/HTNWorldState.h"

DEFINE_LOG_CATEGORY(LogHTN);


bool FHTNCondition::Evaluate(class UHTNWorldState* WorldState) const
{
	if (!WorldState) return false;
    
	FHTNWorldProperty Property = WorldState->GetProperty(PropertyName);
    
	if (CheckNumeric)
	{
		return Property.NumericValue >= NumericThreshold;
	}
	else
	{
		return Property.Value == ExpectedValue;
	}
}
