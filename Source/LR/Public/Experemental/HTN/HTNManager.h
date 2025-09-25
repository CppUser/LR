// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/HTNMultiAgentCoordinator.h"
#include "AI/HTNUtilityAIBridge.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HTNManager.generated.h"


UCLASS()
class LR_API UHTNManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "HTN")
	UHTNExecutor* CreateExecutor(FName ExecutorID);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void DestroyExecutor(FName ExecutorID);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	UHTNPlanner* CreatePlanner(EHTNPlannerMode Mode = EHTNPlannerMode::Dynamic);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void TickHTNSystem(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void RegisterTaskClass(TSubclassOf<UHTNTask> TaskClass, FName TaskID);

	UFUNCTION(BlueprintCallable, Category = "HTN", meta = (DisplayName = "Get HTN Manager"))
	static UHTNManager* GetHTNManager(const UObject* WorldContext);

public:
	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	TMap<FName, UHTNExecutor*> ActiveExecutors;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	UHTNMultiAgentCoordinator* MultiAgentCoordinator = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	UHTNWorldState* GlobalWorldState = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "HTN")
	UHTNUtilitySelector* UtilitySelector = nullptr;

protected:
	TMap<FName, TSubclassOf<UHTNTask>> RegisteredTasks;
	FTimerHandle TickTimerHandle;
};
