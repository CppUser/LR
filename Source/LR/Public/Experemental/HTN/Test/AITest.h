// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Experemental/HTN/HTNCompoundTask.h"
#include "Experemental/HTN/HTNPrimitiveTask.h"

#include "AITest.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LR_API UAITask_MoveToLocation  : public UHTNPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual void OnExecuteAction_Implementation(UHTNWorldState* WorldState) override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual float CalculateUtility_Implementation(UHTNWorldState* WorldState) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AcceptanceRadius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	class AAIController* AIController = nullptr;

protected:
	bool bIsMoving = false;
};


UCLASS(BlueprintType, Blueprintable)
class LR_API UAITask_Combat  : public UHTNCompoundTask
{
	GENERATED_BODY()
public:
	UAITask_Combat();

	
	virtual FHTNMethod SelectBestMethod_Implementation(
		const TArray<FHTNMethod>& ValidMethods, 
		UHTNWorldState* WorldState
	) const override;
	
protected:
	void SetupCombatMethods();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* Target = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float EngagementRange = 1000.0f;
};