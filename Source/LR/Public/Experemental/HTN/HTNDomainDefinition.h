// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "Engine/DataAsset.h"
#include "HTNDomainDefinition.generated.h"


class UHTNConfig;
struct FHTNReactiveRule;

USTRUCT(BlueprintType)
struct FHTNDomainTask
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TaskID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UHTNTask> TaskClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNCondition> DefaultPreconditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNEffect> DefaultEffects;
};

UCLASS(BlueprintType)
class LR_API UHTNDomainDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Domain")
	UHTNTask* CreateDomainTask(FName TaskID);

	UFUNCTION(BlueprintCallable, Category = "Domain")
	UHTNWorldState* CreateDomainWorldState();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Domain")
	FName DomainName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Domain")
	TArray<FHTNDomainTask> DomainTasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Domain")
	TMap<FName, FHTNWorldProperty> DefaultWorldProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Domain")
	TArray<FHTNReactiveRule> DefaultReactiveRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Domain")
	UHTNConfig* Configuration = nullptr;
};
