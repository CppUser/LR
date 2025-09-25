// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTypes.h"
#include "UObject/Object.h"
#include "HTNTask.generated.h"

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
