// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HTNTask.h"
#include "UObject/Object.h"
#include "SmartObjectSubsystem.h"
#include "HTNPrimitiveTask.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNPrimitiveTask : public UHTNTask
{
	GENERATED_BODY()
public:
	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Interrupt_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	void OnExecuteAction(UHTNWorldState* WorldState);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	float ExecutionTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	bool bIsInterruptible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	float SuccessProbability = 1.0f;

protected:
	float CurrentExecutionTime = 0.0f;
};



UCLASS(BlueprintType)
class LR_API UHTNSmartObjectTask : public UHTNPrimitiveTask
{
	GENERATED_BODY()

public:
	
	virtual void OnExecuteAction_Implementation(UHTNWorldState* WorldState) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmartObject")
	FGameplayTagQuery SmartObjectQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmartObject")
	float SearchRadius = 1000.0f;

protected:
	UPROPERTY()
	USmartObjectSubsystem* SmartObjectSubsystem = nullptr;

	FSmartObjectClaimHandle ClaimedObject;
};