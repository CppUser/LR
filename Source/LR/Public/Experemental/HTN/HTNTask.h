// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTypes.h"
#include "UObject/Object.h"
#include "MassEntitySubsystem.h"
#include "HTNTask.generated.h"

class UHTNPlanner;
class UHTNWorldState;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHTNTaskDelegate, class UHTNTask*, Task, EHTNTaskStatus, Status);

UCLASS(Abstract, BlueprintType, Blueprintable)
class LR_API UHTNTask : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	bool CanExecute(UHTNWorldState* WorldState) const;

	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	void Execute(UHTNWorldState* WorldState);

	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	void Interrupt();

	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	float CalculateUtility(UHTNWorldState* WorldState) const;

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void SetStatus(EHTNTaskStatus NewStatus);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	FName TaskName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	float UtilityScore = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	TArray<FHTNCondition> Preconditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	TArray<FHTNEffect> Effects;

	UPROPERTY(BlueprintReadOnly, Category = "HTN")
	EHTNTaskStatus Status = EHTNTaskStatus::None;

	UPROPERTY(BlueprintAssignable)
	FHTNTaskDelegate OnTaskCompleted;
};

/////////////////////////////////////////////////////////////////////////////////////////

UCLASS(BlueprintType)
class LR_API UHTNTaskPool : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void InitializePool(TSubclassOf<UHTNTask> TaskClass);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	UHTNTask* GetTask(TSubclassOf<UHTNTask> TaskClass);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void ReturnTask(UHTNTask* Task);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void ClearPool();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
	int32 InitialPoolSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
	int32 MaxPoolSize = 200;
protected:
	TMap<UClass*, TArray<UHTNTask*>> AvailableTasks;
	TMap<UClass*, TArray<UHTNTask*>> InUseTasks;
};





UCLASS(BlueprintType)
class LR_API UHTNMassEntityTask : public UHTNTask
{
	GENERATED_BODY()

public:
	
	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;
protected:
	void ProcessEntities(UMassEntitySubsystem* EntitySubsystem);
public:
	FMassEntityQuery EntityQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mass")
	int32 MaxEntitiesToProcess = 100;


};

