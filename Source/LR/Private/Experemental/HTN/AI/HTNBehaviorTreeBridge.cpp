// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/AI/HTNBehaviorTreeBridge.h"

#include "AIController.h"

void UHTNBehaviorTreeBridge::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

	if (!BehaviorTree || !AIController)
	{
		UE_LOG(LogHTN, Error, TEXT("BehaviorTreeTask: Missing BehaviorTree or AIController"));
		SetStatus(EHTNTaskStatus::Failed);
		return;
	}
    
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(
		AIController->GetBrainComponent());
    
	if (!BTComponent)
	{
		UE_LOG(LogHTN, Error, TEXT("BehaviorTreeTask: AIController has no BehaviorTreeComponent"));
		SetStatus(EHTNTaskStatus::Failed);
		return;
	}
    
	//TODO:BTComponent->OnBehaviorTreeCompleted.AddUObject(this, &UHTNBehaviorTreeTask::OnBehaviorTreeCompleted);
    
	BTComponent->StartTree(*BehaviorTree);
    
	UE_LOG(LogHTN, Log, TEXT("Started behavior tree: %s"), *BehaviorTree->GetName());

}

void UHTNBehaviorTreeBridge::Interrupt_Implementation()
{
	if (AIController)
	{
		UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(
			AIController->GetBrainComponent());
        
		if (BTComponent)
		{
			BTComponent->StopTree();
			//TODO:BTComponent->OnBehaviorTreeCompleted.RemoveAll(this);
		}
	}
	
	Super::Interrupt_Implementation();
}

void UHTNBehaviorTreeBridge::OnBehaviorTreeCompleted(UBehaviorTreeComponent& OwnerComp, uint8 NodeMemory)
{
	//TODO:OwnerComp.OnBehaviorTreeCompleted.RemoveAll(this);
	SetStatus(EHTNTaskStatus::Success);
}
