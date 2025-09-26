// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNPrototyping.h"

#include "Experemental/HTN/HTNWorldState.h"

void UHTNMemorySystem::StoreMemory(const FHTNMemoryEntry& Memory)
{
	WorkingMemory.Add(Memory);
    
	if (WorkingMemory.Num() > MaxMemoryEntries)
	{
		int32 RemoveIndex = 0;
		float LowestImportance = WorkingMemory[0].ImportanceScore;
        
		for (int32 i = 1; i < WorkingMemory.Num(); i++)
		{
			if (WorkingMemory[i].ImportanceScore < LowestImportance)
			{
				LowestImportance = WorkingMemory[i].ImportanceScore;
				RemoveIndex = i;
			}
		}
        
		WorkingMemory.RemoveAt(RemoveIndex);
	}
}

TArray<FHTNMemoryEntry> UHTNMemorySystem::RecallMemories(FName MemoryType, int32 MaxResults)
{
	TArray<FHTNMemoryEntry> Results;
    
	for (const FHTNMemoryEntry& Memory : WorkingMemory)
	{
		if (Memory.MemoryType == MemoryType)
		{
			Results.Add(Memory);
            
			if (Results.Num() >= MaxResults)
			{
				break;
			}
		}
	}
    
	Results.Sort([](const FHTNMemoryEntry& A, const FHTNMemoryEntry& B)
	{
		float ScoreA = A.ImportanceScore * (1.0f / (FDateTime::Now() - A.Timestamp).GetTotalHours());
		float ScoreB = B.ImportanceScore * (1.0f / (FDateTime::Now() - B.Timestamp).GetTotalHours());
		return ScoreA > ScoreB;
	});
    
	return Results;
}

FHTNMemoryEntry UHTNMemorySystem::FindSimilarSituation(UHTNWorldState* CurrentState)
{
	if (!CurrentState || WorkingMemory.Num() == 0)
	{
		return FHTNMemoryEntry();
	}
    
	FHTNMemoryEntry BestMatch;
	float BestSimilarity = 0.0f;
    
	for (const FHTNMemoryEntry& Memory : WorkingMemory)
	{
		float Similarity = CalculateSimilarity(Memory, CurrentState);
        
		if (Similarity > BestSimilarity)
		{
			BestSimilarity = Similarity;
			BestMatch = Memory;
		}
	}
    
	return BestMatch;
}

void UHTNMemorySystem::UpdateMemoryImportance(const FHTNMemoryEntry& Memory, float DeltaImportance)
{
	for (FHTNMemoryEntry& StoredMemory : WorkingMemory)
	{
		if (StoredMemory.Timestamp == Memory.Timestamp && 
			StoredMemory.TaskExecuted == Memory.TaskExecuted)
		{
			StoredMemory.ImportanceScore = FMath::Clamp(
				StoredMemory.ImportanceScore + DeltaImportance, 0.0f, 10.0f);
			break;
		}
	}
}

void UHTNMemorySystem::ApplyMemoryDecay()
{
	FDateTime Now = FDateTime::Now();
    
	for (FHTNMemoryEntry& Memory : WorkingMemory)
	{
		float HoursSinceMemory = (Now - Memory.Timestamp).GetTotalHours();
		float DecayFactor = FMath::Exp(-MemoryDecayRate * HoursSinceMemory);
		Memory.ImportanceScore *= DecayFactor;
	}
    
	WorkingMemory.RemoveAll([](const FHTNMemoryEntry& Memory)
	{
		return Memory.ImportanceScore < 0.01f;
	});
}

void UHTNMemorySystem::SaveToPersistentStorage()
{
	if (!PersistentMemoryTable) return;
    
	PersistentMemoryTable->EmptyTable();
    
	for (int32 i = 0; i < WorkingMemory.Num(); i++)
	{
		FName RowName = FName(*FString::Printf(TEXT("Memory_%d"), i));
		PersistentMemoryTable->AddRow(RowName, WorkingMemory[i]);
	}
}

void UHTNMemorySystem::LoadFromPersistentStorage()
{
	if (!PersistentMemoryTable) return;
    
	WorkingMemory.Empty();
    
	TArray<FHTNMemoryEntry*> AllRows;
	PersistentMemoryTable->GetAllRows<FHTNMemoryEntry>(TEXT("LoadMemories"), AllRows);
    
	for (FHTNMemoryEntry* Row : AllRows)
	{
		if (Row)
		{
			WorkingMemory.Add(*Row);
		}
	}
}

float UHTNMemorySystem::CalculateSimilarity(const FHTNMemoryEntry& Memory, UHTNWorldState* State)
{
	if (!State) return 0.0f;
    
	float TotalSimilarity = 0.0f;
	int32 ComparedProperties = 0;
    
	for (const auto& MemProp : Memory.StateSnapshot)
	{
		FHTNWorldProperty CurrentProp = State->GetProperty(MemProp.Key);
        
		if (State->HasProperty(MemProp.Key))
		{
			ComparedProperties++;
            
			if (MemProp.Value.BoolValue == CurrentProp.BoolValue)
			{
				TotalSimilarity += 0.5f;
			}
            
			if (MemProp.Value.Value == CurrentProp.Value)
			{
				TotalSimilarity += 0.3f;
			}
            
			float NumericDiff = FMath::Abs(MemProp.Value.NumericValue - CurrentProp.NumericValue);
			float NumericSimilarity = FMath::Max(0.0f, 1.0f - NumericDiff / 100.0f);
			TotalSimilarity += NumericSimilarity * 0.2f;
		}
	}
    
	return ComparedProperties > 0 ? TotalSimilarity / ComparedProperties : 0.0f;

}
