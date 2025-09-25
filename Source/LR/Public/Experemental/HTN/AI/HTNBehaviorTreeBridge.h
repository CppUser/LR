// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Experemental/HTN/HTNPrimitiveTask.h"
#include "BehaviorTree/BehaviorTree.h"
#include "HTNBehaviorTreeBridge.generated.h"

/**
 * 
 */
UCLASS()
class LR_API UHTNBehaviorTreeBridge : public UHTNPrimitiveTask
{
	GENERATED_BODY()
public:
	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;
	virtual void Interrupt_Implementation() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BT")
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BT")
	class AAIController* AIController = nullptr;
protected:
	void OnBehaviorTreeCompleted(UBehaviorTreeComponent& OwnerComp, uint8 NodeMemory);
};
