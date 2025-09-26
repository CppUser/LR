// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTypes.h"
#include "Engine/DataAsset.h"
#include "HTNConfig.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LR_API UHTNConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	EHTNPlannerMode DefaultPlannerMode = EHTNPlannerMode::Dynamic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	int32 MaxPlanDepth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	float PlanTimeout = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bEnableAsyncPlanning = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bEnableMultiAgent = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bEnableReactivePlanning = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bEnableMemorySystem = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bEnableMachineLearning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Performance")
	int32 TaskPoolInitialSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Performance")
	int32 TaskPoolMaxSize = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Performance")
	float TickInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Debug")
	bool bEnableDebugVisualization = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Debug")
	bool bLogPlanningSteps = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Debug")
	bool bLogExecutionSteps = false;
};
