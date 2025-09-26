// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNPrimitiveTask.h"
#include "SmartObjectSubsystem.h"
#include "GameplayTagContainer.h"
#include "Experemental/HTN/HTNWorldState.h"

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

	//TODO: Fix implementation
	
	// UWorld* World = GetWorld();
	// if (!World) 
	// {
	// 	SetStatus(EHTNTaskStatus::Failed);
	// 	return;
	// }
 //    
	// SmartObjectSubsystem = USmartObjectSubsystem::GetCurrent(World);
	// if (!SmartObjectSubsystem)
	// {
	// 	UE_LOG(LogHTN, Error, TEXT("SmartObjectSubsystem not found"));
	// 	SetStatus(EHTNTaskStatus::Failed);
	// 	return;
	// }
 //    
	// FSmartObjectRequest Request;
	// Request.QueryBox = FBox(FVector(-SearchRadius), FVector(SearchRadius));
	// Request.Filter.UserTags = SmartObjectQuery.GetGameplayTagArray();
 //    
	// TArray<FSmartObjectRequestResult> Results;
	// SmartObjectSubsystem->FindSmartObjects(Request, Results);
 //    
	// if (Results.Num() == 0)
	// {
	// 	UE_LOG(LogHTN, Warning, TEXT("No smart objects found matching query"));
	// 	SetStatus(EHTNTaskStatus::Failed);
	// 	return;
	// }
 //    
	// for (const FSmartObjectRequestResult& Result : Results)
	// {
	// 	ClaimedObject = SmartObjectSubsystem->Claim(Result);
	// 	if (ClaimedObject.IsValid())
	// 	{
	// 		UE_LOG(LogHTN, Log, TEXT("Claimed smart object"));
 //            
	// 		FHTNWorldProperty SmartObjProperty;
	// 		SmartObjProperty.PropertyName = TEXT("SmartObjectClaimed");
	// 		SmartObjProperty.BoolValue = true;
	// 		WorldState->SetProperty(SmartObjProperty.PropertyName, SmartObjProperty);
 //            
	// 		SmartObjectSubsystem->Use(ClaimedObject);
 //            
	// 		SetStatus(EHTNTaskStatus::Success);
	// 		return;
	// 	}
	// }
 //    
	// SetStatus(EHTNTaskStatus::Failed);
}