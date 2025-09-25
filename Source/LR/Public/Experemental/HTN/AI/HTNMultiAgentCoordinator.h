// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Experemental/HTN/HTNExecutor.h"
#include "UObject/Object.h"
#include "HTNMultiAgentCoordinator.generated.h"

USTRUCT(BlueprintType)
struct FHTNAgentInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName AgentID;

	UPROPERTY(BlueprintReadWrite)
	UHTNExecutor* Executor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UHTNWorldState* LocalWorldState = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FName> Capabilities;

	UPROPERTY(BlueprintReadWrite)
	float CooperationScore = 1.0f;
};

USTRUCT(BlueprintType)
struct FHTNSharedTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UHTNTask* Task = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FName> AssignedAgents;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, UHTNTask*> AgentSubtasks;
};


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNMultiAgentCoordinator : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HTN")
	void RegisterAgent(FName AgentID, UHTNExecutor* Executor, const TArray<FName>& Capabilities);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void UnregisterAgent(FName AgentID);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void AssignSharedTask(UHTNTask* Task);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	TArray<FName> SelectBestAgentsForTask(UHTNTask* Task, int32 MaxAgents = 1);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void CoordinatePlans();

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void SynchronizeWorldStates();

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void Tick(float DeltaTime);
public:
	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	TMap<FName, FHTNAgentInfo> Agents;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	UHTNWorldState* SharedWorldState = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	TArray<FHTNSharedTask> SharedTasks;
};
