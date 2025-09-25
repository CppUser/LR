// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Experemental/HTN/HTNCompoundTask.h"
#include "UObject/Object.h"
#include "WorldTest.generated.h"

USTRUCT(BlueprintType)
struct FWorldEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Intensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNEffect> WorldEffects;
};



UCLASS(BlueprintType, Blueprintable)
class LR_API UWorldTest : public UHTNCompoundTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "World")
	void TriggerWorldEvent(const FWorldEvent& Event);

	UFUNCTION(BlueprintCallable, Category = "World")
	TArray<UHTNTask*> GenerateResponseTasks(const FWorldEvent& Event);

	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	TArray<FWorldEvent> ActiveEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	float EventCheckInterval = 1.0f;
};
