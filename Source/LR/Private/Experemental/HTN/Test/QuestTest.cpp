// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/Test/QuestTest.h"

#include "Experemental/HTN/HTNWorldState.h"

//////////////////////////////////////////////
///
/////////////////////////////////////////////
void UQuestTask::GenerateQuestMethods()
{
	Methods.Empty();
    
    FHTNMethod MainPathMethod;
    MainPathMethod.MethodName = TEXT("MainQuestPath");
    MainPathMethod.PreferenceScore = 1.0f;
    
    FHTNCondition QuestActiveCondition;
    QuestActiveCondition.PropertyName = FName(*FString::Printf(TEXT("Quest_%s_Active"), *QuestID.ToString()));
    QuestActiveCondition.ExpectedValue = TEXT("true");
    MainPathMethod.Preconditions.Add(QuestActiveCondition);
    
    for (const FQuestObjective& Objective : Objectives)
    {
        if (Objective.bCompleted) continue;
        
        // Create subtasks based on objective type
        // TODO: have a registry to map objective types to task classes
        switch (Objective.Type)
        {
        case EQuestObjectiveType::Kill:
            {
                FHTNCondition CombatCondition;
                CombatCondition.PropertyName = TEXT("CanEngageCombat");
                //TODO:CombatCondition.BoolValue = true;
                MainPathMethod.Preconditions.Add(CombatCondition);
            }
            break;
            
        case EQuestObjectiveType::Collect:
            {
                FHTNCondition CollectCondition;
                CollectCondition.PropertyName = TEXT("CanCollectItems");
                //TODO:CollectCondition.BoolValue = true;
                MainPathMethod.Preconditions.Add(CollectCondition);
            }
            break;
            
        case EQuestObjectiveType::Deliver:
            {
                FHTNCondition DeliverCondition;
                DeliverCondition.PropertyName = TEXT("HasDeliveryItem");
                //TODO:DeliverCondition.BoolValue = true;
                MainPathMethod.Preconditions.Add(DeliverCondition);
            }
            break;
            
        case EQuestObjectiveType::Interact:
            {
                FHTNCondition InteractCondition;
                InteractCondition.PropertyName = TEXT("CanInteract");
                //TODO:InteractCondition.BoolValue = true;
                MainPathMethod.Preconditions.Add(InteractCondition);
            }
            break;
            
        case EQuestObjectiveType::Escort:
            {
                FHTNCondition EscortCondition;
                EscortCondition.PropertyName = TEXT("EscortTargetNearby");
                //TODO:EscortCondition.BoolValue = true;
                MainPathMethod.Preconditions.Add(EscortCondition);
            }
            break;
            
        case EQuestObjectiveType::Survive:
            {
                FHTNCondition SurviveCondition;
                SurviveCondition.PropertyName = TEXT("Health");
                SurviveCondition.CheckNumeric = true;
                SurviveCondition.NumericThreshold = 10.0f;
                MainPathMethod.Preconditions.Add(SurviveCondition);
            }
            break;
        }
    }
    
    Methods.Add(MainPathMethod);
    
    if (OptionalObjectives.Num() > 0)
    {
        FHTNMethod OptionalPathMethod;
        OptionalPathMethod.MethodName = TEXT("OptionalQuestPath");
        OptionalPathMethod.PreferenceScore = 0.5f;
        
        for (const FQuestObjective& OptionalObj : OptionalObjectives)
        {
            FHTNCondition OptionalCondition;
            OptionalCondition.PropertyName = FName(*FString::Printf(TEXT("Optional_%s_Available"), 
                                                                   *OptionalObj.ObjectiveID.ToString()));
            //TODO:OptionalCondition.BoolValue = true;
            OptionalPathMethod.Preconditions.Add(OptionalCondition);
        }
        
        Methods.Add(OptionalPathMethod);
    }
}

float UQuestTask::GetQuestProgress() const
{
    if (Objectives.Num() == 0) return 1.0f;
    
    int32 CompletedCount = 0;
    int32 TotalRequired = 0;
    
    for (const FQuestObjective& Objective : Objectives)
    {
        TotalRequired += Objective.RequiredCount;
        CompletedCount += Objective.CurrentCount;
    }
    
    return TotalRequired > 0 ? (float)CompletedCount / TotalRequired : 0.0f;

}

void UQuestTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

    if (!WorldState) return;
    
    FHTNWorldProperty QuestProperty;
    QuestProperty.PropertyName = FName(*FString::Printf(TEXT("Quest_%s"), *QuestID.ToString()));
    QuestProperty.Value = TEXT("Active");
    QuestProperty.NumericValue = GetQuestProgress();
    WorldState->SetProperty(QuestProperty.PropertyName, QuestProperty);
    
    for (const FQuestObjective& Objective : Objectives)
    {
        FHTNWorldProperty ObjProperty;
        ObjProperty.PropertyName = FName(*FString::Printf(TEXT("Objective_%s"), *Objective.ObjectiveID.ToString()));
        ObjProperty.NumericValue = (float)Objective.CurrentCount / Objective.RequiredCount;
        ObjProperty.BoolValue = Objective.bCompleted;
        WorldState->SetProperty(ObjProperty.PropertyName, ObjProperty);
    }
    
    UE_LOG(LogHTN, Log, TEXT("Executing quest: %s (Progress: %.1f%%)"), 
           *QuestID.ToString(), GetQuestProgress() * 100.0f);
}
//////////////////////////////////////////////
///
/////////////////////////////////////////////