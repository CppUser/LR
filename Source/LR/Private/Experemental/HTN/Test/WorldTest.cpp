// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/Test/WorldTest.h"

#include "Experemental/HTN/HTNPrimitiveTask.h"
#include "Experemental/HTN/HTNWorldState.h"

void UWorldTest::TriggerWorldEvent(const FWorldEvent& Event)
{
	ActiveEvents.Add(Event);
    
	UE_LOG(LogHTN, Log, TEXT("World event triggered: %s at %s (Intensity: %.2f)"),
		   *Event.EventName.ToString(),
		   *Event.Location.ToString(),
		   Event.Intensity);
    
	TArray<UHTNTask*> ResponseTasks = GenerateResponseTasks(Event);
    
	for (int32 i = 0; i < ResponseTasks.Num(); i++)
	{
		FHTNMethod ResponseMethod;
		ResponseMethod.MethodName = FName(*FString::Printf(TEXT("Response_%s_%d"), 
														  *Event.EventName.ToString(), i));
		ResponseMethod.PreferenceScore = Event.Intensity;
        
		FHTNCondition EventCondition;
		EventCondition.PropertyName = FName(*FString::Printf(TEXT("Event_%s_Active"), 
															*Event.EventName.ToString()));
		//TODO:EventCondition.BoolValue = true;
		ResponseMethod.Preconditions.Add(EventCondition);
        
		FHTNCondition DistanceCondition;
		DistanceCondition.PropertyName = TEXT("DistanceToEvent");
		DistanceCondition.CheckNumeric = true;
		DistanceCondition.NumericThreshold = 5000.0f; // Within 50 meters
		ResponseMethod.Preconditions.Add(DistanceCondition);
        
		Methods.Add(ResponseMethod);
	}
}

TArray<UHTNTask*> UWorldTest::GenerateResponseTasks(const FWorldEvent& Event)
{
	TArray<UHTNTask*> ResponseTasks;
    
	if (Event.EventName.ToString().Contains(TEXT("Fire")))
	{
		UHTNPrimitiveTask* ExtinguishTask = NewObject<UHTNPrimitiveTask>(this);
		ExtinguishTask->TaskName = TEXT("ExtinguishFire");
		ExtinguishTask->Priority = Event.Intensity * 10;
		ExtinguishTask->ExecutionTime = 5.0f;
        
		FHTNEffect ExtinguishEffect;
		ExtinguishEffect.PropertyName = FName(*FString::Printf(TEXT("Event_%s_Active"), 
															  *Event.EventName.ToString()));
		ExtinguishEffect.Operator = EHTNOperatorType::Set;
		ExtinguishEffect.NewValue.BoolValue = false;
		ExtinguishTask->Effects.Add(ExtinguishEffect);
        
		ResponseTasks.Add(ExtinguishTask);
	}
	else if (Event.EventName.ToString().Contains(TEXT("Attack")))
	{
		UHTNPrimitiveTask* DefendTask = NewObject<UHTNPrimitiveTask>(this);
		DefendTask->TaskName = TEXT("DefendArea");
		DefendTask->Priority = Event.Intensity * 15;
		DefendTask->ExecutionTime = 10.0f;
        
		ResponseTasks.Add(DefendTask);
	}
	else if (Event.EventName.ToString().Contains(TEXT("Resource")))
	{
		UHTNPrimitiveTask* GatherTask = NewObject<UHTNPrimitiveTask>(this);
		GatherTask->TaskName = TEXT("GatherResources");
		GatherTask->Priority = Event.Intensity * 5;
		GatherTask->ExecutionTime = 3.0f;
        
		ResponseTasks.Add(GatherTask);
	}
    
	return ResponseTasks;
}

void UWorldTest::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

	if (!WorldState) return;
    
	for (const FWorldEvent& Event : ActiveEvents)
	{
		FHTNWorldProperty EventProperty;
		EventProperty.PropertyName = FName(*FString::Printf(TEXT("Event_%s_Active"), 
														   *Event.EventName.ToString()));
		EventProperty.BoolValue = true;
		EventProperty.NumericValue = Event.Intensity;
		WorldState->SetProperty(EventProperty.PropertyName, EventProperty);
        
		for (const FHTNEffect& Effect : Event.WorldEffects)
		{
			WorldState->ApplyEffect(Effect);
		}
	}
    
	// Schedule periodic event checks
	// TODO: handle by a timer or tick system
}
