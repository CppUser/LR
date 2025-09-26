// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTypes.h"
#include "HTNPrototyping.generated.h"

class UHTNWorldState;

USTRUCT(BlueprintType)
struct FHTNExperience
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> StateFeatures;

	UPROPERTY()
	FName SelectedMethod;

	UPROPERTY()
	float Reward = 0.0f;

	UPROPERTY()
	bool bSuccessful = false;
};

// UCLASS(BlueprintType)
// class LR_API UHTNMLAdapter : public UObject
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ML")
// 	UNeuralNetwork* PolicyNetwork = nullptr;
//
// 	UFUNCTION(BlueprintCallable, Category = "ML")
// 	FName PredictBestMethod(const TArray<FHTNMethod>& Methods, UHTNWorldState* WorldState);
//
// 	UFUNCTION(BlueprintCallable, Category = "ML")
// 	void RecordExperience(const FHTNExperience& Experience);
//
// 	UFUNCTION(BlueprintCallable, Category = "ML")
// 	void TrainFromExperiences();
//
// 	UFUNCTION(BlueprintCallable, Category = "ML")
// 	TArray<float> ExtractStateFeatures(UHTNWorldState* WorldState);
// public:
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ML")
// 	float LearningRate = 0.001f;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ML")
// 	float ExplorationRate = 0.1f;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ML")
// 	int32 ExperienceBufferSize = 1000;
//
// protected:
// 	TArray<FHTNExperience> ExperienceBuffer;
//     
// 	void UpdatePolicyNetwork();
// };

/////////////////////////////////////////////////////////////////
///  MemorySystem - Long-term memory for persistent planning
/////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FHTNMemoryEntry : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MemoryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FHTNWorldProperty> StateSnapshot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TaskExecuted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSuccessful = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImportanceScore = 1.0f;
};


UCLASS(BlueprintType)
class LR_API UHTNMemorySystem : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void StoreMemory(const FHTNMemoryEntry& Memory);

	UFUNCTION(BlueprintCallable, Category = "Memory")
	TArray<FHTNMemoryEntry> RecallMemories(FName MemoryType, int32 MaxResults = 10);

	UFUNCTION(BlueprintCallable, Category = "Memory")
	FHTNMemoryEntry FindSimilarSituation(UHTNWorldState* CurrentState);

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void UpdateMemoryImportance(const FHTNMemoryEntry& Memory, float DeltaImportance);

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void ApplyMemoryDecay();

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void SaveToPersistentStorage();

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void LoadFromPersistentStorage();
protected:
	float CalculateSimilarity(const FHTNMemoryEntry& Memory, UHTNWorldState* State);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	int32 MaxMemoryEntries = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	float MemoryDecayRate = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	UDataTable* PersistentMemoryTable = nullptr;

protected:
	TArray<FHTNMemoryEntry> WorkingMemory;
};

UCLASS()
class LR_API UHTNPrototyping : public UObject
{
	GENERATED_BODY()
};
