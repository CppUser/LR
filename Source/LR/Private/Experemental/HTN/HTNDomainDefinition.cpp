// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNDomainDefinition.h"

#include "Experemental/HTN/HTNWorldState.h"

UHTNTask* UHTNDomainDefinition::CreateDomainTask(FName TaskID)
{
	const FHTNDomainTask* TaskDef = nullptr;
	for (const FHTNDomainTask& DomainTask : DomainTasks)
	{
		if (DomainTask.TaskID == TaskID)
		{
			TaskDef = &DomainTask;
			break;
		}
	}
    
	if (!TaskDef || !TaskDef->TaskClass)
	{
		UE_LOG(LogHTN, Error, TEXT("Domain task not found: %s"), *TaskID.ToString());
		return nullptr;
	}
    
	UHTNTask* Task = NewObject<UHTNTask>(GetTransientPackage(), TaskDef->TaskClass);
	Task->TaskName = TaskID;
	Task->Preconditions = TaskDef->DefaultPreconditions;
	Task->Effects = TaskDef->DefaultEffects;
    
	return Task;
}

UHTNWorldState* UHTNDomainDefinition::CreateDomainWorldState()
{
	UHTNWorldState* WorldState = NewObject<UHTNWorldState>(GetTransientPackage());
    
	for (const auto& PropPair : DefaultWorldProperties)
	{
		WorldState->SetProperty(PropPair.Key, PropPair.Value);
	}
    
	return WorldState;
}
