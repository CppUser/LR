#pragma once

#include "HTNTask.h"
#include "HTNCompoundTask.generated.h"

USTRUCT(BlueprintType)
struct FHTNMethod
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MethodName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHTNCondition> Preconditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UHTNTask>> Subtasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PreferenceScore = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowParallelExecution = false;
};


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNCompoundTask : public UHTNTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HTN")
	TArray<FHTNMethod> GetValidMethods(UHTNWorldState* WorldState) const;

	UFUNCTION(BlueprintNativeEvent, Category = "HTN")
	FHTNMethod SelectBestMethod(const TArray<FHTNMethod>& ValidMethods, UHTNWorldState* WorldState) const;

	virtual void Execute_Implementation(UHTNWorldState* WorldState) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN")
	TArray<FHTNMethod> Methods;
};
