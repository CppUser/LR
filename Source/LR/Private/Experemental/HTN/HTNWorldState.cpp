// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNWorldState.h"


void UHTNWorldState::SetProperty(FName PropertyName, const FHTNWorldProperty& Value)
{
	Properties.Add(PropertyName, Value);
	StateVersion++;
}

FHTNWorldProperty UHTNWorldState::GetProperty(FName PropertyName) const
{
	const FHTNWorldProperty* Found = Properties.Find(PropertyName);
	if (Found)
	{
		return *Found;
	}

	FHTNWorldProperty Empty;
	Empty.PropertyName = PropertyName;
	return Empty;
}

bool UHTNWorldState::HasProperty(FName PropertyName) const
{
	return Properties.Contains(PropertyName);
}

void UHTNWorldState::ApplyEffect(const FHTNEffect& Effect)
{
	FHTNWorldProperty* Property = Properties.Find(Effect.PropertyName);
    
	switch (Effect.Operator)
	{
	case EHTNOperatorType::Add:
		if (Property)
		{
			Property->NumericValue += Effect.NewValue.NumericValue;
		}
		else
		{
			Properties.Add(Effect.PropertyName, Effect.NewValue);
		}
		break;
        
	case EHTNOperatorType::Remove:
		Properties.Remove(Effect.PropertyName);
		break;
        
	case EHTNOperatorType::Set:
		Properties.Add(Effect.PropertyName, Effect.NewValue);
		break;
	}
    
	StateVersion++;
}

UHTNWorldState* UHTNWorldState::Clone() const
{
	UHTNWorldState* NewState = NewObject<UHTNWorldState>(GetTransientPackage());
	NewState->CopyFrom(this);
	return NewState;
}

void UHTNWorldState::CopyFrom(const UHTNWorldState* Other)
{
	if (!Other) return;
    
	Properties = Other->Properties;
	StateVersion = Other->StateVersion;
}

float UHTNWorldState::CalculateDifference(const UHTNWorldState* Other) const
{
	if (!Other) return 1.0f;
    
	float TotalDifference = 0.0f;
	int32 PropertyCount = 0;

	for (const auto& Pair : Properties)
	{
		PropertyCount++;
		const FHTNWorldProperty* OtherProp = Other->Properties.Find(Pair.Key);
        
		if (!OtherProp)
		{
			TotalDifference += 1.0f;
		}
		else
		{
			if (Pair.Value.BoolValue != OtherProp->BoolValue)
			{
				TotalDifference += 1.0f;
			}
			else if (Pair.Value.Value != OtherProp->Value)
			{
				TotalDifference += 0.5f;
			}
			else if (!FMath::IsNearlyEqual(Pair.Value.NumericValue, OtherProp->NumericValue, 0.01f))
			{
				float NumDiff = FMath::Abs(Pair.Value.NumericValue - OtherProp->NumericValue);
				TotalDifference += FMath::Min(NumDiff / 100.0f, 1.0f);
			}
		}
	}

	for (const auto& Pair : Other->Properties)
	{
		if (!Properties.Contains(Pair.Key))
		{
			PropertyCount++;
			TotalDifference += 1.0f;
		}
	}
    
	return PropertyCount > 0 ? TotalDifference / PropertyCount : 0.0f;

}
