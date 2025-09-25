// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Experemental/HTN/HTNCompoundTask.h"
#include "UObject/Object.h"
#include "QuestTest.generated.h"



UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	Kill,
	Collect,
	Deliver,
	Interact,
	Escort,
	Survive
};


USTRUCT(BlueprintType)
struct FQuestObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ObjectiveID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestObjectiveType Type = EQuestObjectiveType::Kill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCompleted = false;
};



UCLASS(BlueprintType, Blueprintable)
class LR_API UQuestTask : public UHTNCompoundTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void GenerateQuestMethods();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	float GetQuestProgress() const;

	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<FQuestObjective> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<FQuestObjective> OptionalObjectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	float QuestPriority = 1.0f;
};
