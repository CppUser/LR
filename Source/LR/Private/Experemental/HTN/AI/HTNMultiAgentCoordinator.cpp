// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/AI/HTNMultiAgentCoordinator.h"

#include "Experemental/HTN/HTNCompoundTask.h"
#include "Experemental/HTN/HTNWorldState.h"

void UHTNMultiAgentCoordinator::RegisterAgent(FName AgentID, UHTNExecutor* Executor, const TArray<FName>& Capabilities)
{
    if (!Executor) return;
    
    FHTNAgentInfo AgentInfo;
    AgentInfo.AgentID = AgentID;
    AgentInfo.Executor = Executor;
    AgentInfo.Capabilities = Capabilities;
    AgentInfo.LocalWorldState = Executor->WorldState;
    AgentInfo.CooperationScore = 1.0f;
    
    Agents.Add(AgentID, AgentInfo);
    
    UE_LOG(LogHTN, Log, TEXT("Registered agent: %s with %d capabilities"), 
           *AgentID.ToString(), Capabilities.Num());
}

void UHTNMultiAgentCoordinator::UnregisterAgent(FName AgentID)
{
    Agents.Remove(AgentID);
    
    for (FHTNSharedTask& SharedTask : SharedTasks)
    {
        SharedTask.AssignedAgents.Remove(AgentID);
        SharedTask.AgentSubtasks.Remove(AgentID);
    }
}

void UHTNMultiAgentCoordinator::AssignSharedTask(UHTNTask* Task)
{
	if (!Task) return;
    
    TArray<FName> BestAgents = SelectBestAgentsForTask(Task);
    
    if (BestAgents.Num() == 0)
    {
        UE_LOG(LogHTN, Warning, TEXT("No suitable agents for task: %s"), 
               *Task->TaskName.ToString());
        return;
    }
    
    FHTNSharedTask SharedTask;
    SharedTask.Task = Task;
    SharedTask.AssignedAgents = BestAgents;
    
    if (UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(Task))
    {
        for (const FName& AgentID : BestAgents)
        {
            FHTNAgentInfo* AgentInfo = Agents.Find(AgentID);
            if (!AgentInfo || !AgentInfo->Executor) continue;
            
            UHTNTask* AgentTask = nullptr;
            
            for (const FHTNMethod& Method : CompoundTask->Methods)
            {
                bool bCanHandle = true;
                
                for (TSubclassOf<UHTNTask> SubtaskClass : Method.Subtasks)
                {
                    if (!SubtaskClass) continue;
                    
                    UHTNTask* TestTask = SubtaskClass.GetDefaultObject();
                    bool bHasCapability = false;
                    
                    for (const FName& Capability : AgentInfo->Capabilities)
                    {
                        if (TestTask->TaskName == Capability)
                        {
                            bHasCapability = true;
                            break;
                        }
                    }
                    
                    if (!bHasCapability)
                    {
                        bCanHandle = false;
                        break;
                    }
                }
                
                if (bCanHandle && Method.Subtasks.Num() > 0)
                {
                    AgentTask = NewObject<UHTNTask>(this, Method.Subtasks[0]);
                    break;
                }
            }
            
            if (AgentTask)
            {
                SharedTask.AgentSubtasks.Add(AgentID, AgentTask);
                
                if (AgentInfo->Executor->Planner)
                {
                    UHTNWorldState* StateToUse = AgentInfo->LocalWorldState ? 
                                                 AgentInfo->LocalWorldState : SharedWorldState;
                    
                    FHTNPlan AgentPlan = AgentInfo->Executor->Planner->CreatePlan(
                        AgentTask, StateToUse);
                    
                    AgentInfo->Executor->ExecutePlan(AgentPlan);
                }
            }
        }
    }
    else
    {
        FHTNAgentInfo* BestAgent = Agents.Find(BestAgents[0]);
        if (BestAgent && BestAgent->Executor && BestAgent->Executor->Planner)
        {
            SharedTask.AgentSubtasks.Add(BestAgents[0], Task);
            
            UHTNWorldState* StateToUse = BestAgent->LocalWorldState ? 
                                         BestAgent->LocalWorldState : SharedWorldState;
            
            FHTNPlan Plan = BestAgent->Executor->Planner->CreatePlan(Task, StateToUse);
            BestAgent->Executor->ExecutePlan(Plan);
        }
    }
    
    SharedTasks.Add(SharedTask);
}

TArray<FName> UHTNMultiAgentCoordinator::SelectBestAgentsForTask(UHTNTask* Task, int32 MaxAgents)
{
    TArray<FName> BestAgents;
    
    if (!Task) return BestAgents;
    
    TArray<TPair<FName, float>> ScoredAgents;
    
    for (const auto& AgentPair : Agents)
    {
        float Score = 0.0f;
        const FHTNAgentInfo& AgentInfo = AgentPair.Value;
        
        for (const FName& Capability : AgentInfo.Capabilities)
        {
            if (Capability == Task->TaskName)
            {
                Score += 10.0f;
            }
            else if (Task->TaskName.ToString().Contains(Capability.ToString()))
            {
                Score += 5.0f;
            }
        }
        
        Score *= AgentInfo.CooperationScore;
        
        if (AgentInfo.Executor && AgentInfo.Executor->CurrentTask == nullptr)
        {
            Score *= 1.5f; // Prefer idle agents
        }
        
        if (Score > 0.0f)
        {
            ScoredAgents.Add(TPair<FName, float>(AgentPair.Key, Score));
        }
    }
    
    ScoredAgents.Sort([](const TPair<FName, float>& A, const TPair<FName, float>& B)
    {
        return A.Value > B.Value;
    });
    
    for (int32 i = 0; i < FMath::Min(MaxAgents, ScoredAgents.Num()); i++)
    {
        BestAgents.Add(ScoredAgents[i].Key);
    }
    
    return BestAgents;
}

void UHTNMultiAgentCoordinator::CoordinatePlans()
{
    //TODO: Refactor this function
    TMap<FName, TArray<FHTNEffect>> PlannedEffects;
    
    for (auto& AgentPair : Agents)
    {
        FHTNAgentInfo& AgentInfo = AgentPair.Value;
        if (!AgentInfo.Executor) continue;
        
        TArray<FHTNEffect> AgentEffects;
        
        for (const FHTNPlanStep& Step : AgentInfo.Executor->CurrentPlan.Steps)
        {
            if (Step.Task)
            {
                AgentEffects.Append(Step.Task->Effects);
            }
        }
        
        PlannedEffects.Add(AgentPair.Key, AgentEffects);
    }
    
    for (auto& AgentPair1 : PlannedEffects)
    {
        for (auto& AgentPair2 : PlannedEffects)
        {
            if (AgentPair1.Key == AgentPair2.Key) continue;
            
            for (const FHTNEffect& Effect1 : AgentPair1.Value)
            {
                for (const FHTNEffect& Effect2 : AgentPair2.Value)
                {
                    if (Effect1.PropertyName == Effect2.PropertyName &&
                        Effect1.NewValue.Value != Effect2.NewValue.Value)
                    {
                        FHTNAgentInfo* Agent1 = Agents.Find(AgentPair1.Key);
                        FHTNAgentInfo* Agent2 = Agents.Find(AgentPair2.Key);
                        
                        if (Agent1 && Agent2)
                        {
                            if (Agent1->CooperationScore < Agent2->CooperationScore)
                            {
                                UE_LOG(LogHTN, Log, TEXT("Conflict detected between %s and %s, replanning %s"),
                                       *AgentPair1.Key.ToString(), 
                                       *AgentPair2.Key.ToString(),
                                       *AgentPair1.Key.ToString());
                                
                                //TODO: Trigger replanning for Agent1
                            }
                        }
                    }
                }
            }
        }
    }
}

void UHTNMultiAgentCoordinator::SynchronizeWorldStates()
{
    if (!SharedWorldState) return;
    
    for (auto& AgentPair : Agents)
    {
        FHTNAgentInfo& AgentInfo = AgentPair.Value;
        if (!AgentInfo.LocalWorldState) continue;
        
        for (const auto& PropPair : AgentInfo.LocalWorldState->Properties)
        {
            FName AgentPropertyName = FName(
                FString::Printf(TEXT("%s_%s"), 
                               *AgentPair.Key.ToString(), 
                               *PropPair.Key.ToString()));
            
            SharedWorldState->SetProperty(AgentPropertyName, PropPair.Value);
        }
    }
    
    for (auto& AgentPair : Agents)
    {
        FHTNAgentInfo& AgentInfo = AgentPair.Value;
        if (!AgentInfo.LocalWorldState) continue;
        
        for (const auto& PropPair : SharedWorldState->Properties)
        {
            if (!PropPair.Key.ToString().Contains(TEXT("_")))
            {
                AgentInfo.LocalWorldState->SetProperty(PropPair.Key, PropPair.Value);
            }
        }
    }
}

void UHTNMultiAgentCoordinator::Tick(float DeltaTime)
{
    for (auto& AgentPair : Agents)
    {
        if (AgentPair.Value.Executor)
        {
            AgentPair.Value.Executor->Tick(DeltaTime);
        }
    }
    
    static float TimeSinceLastCoordination = 0.0f;
    TimeSinceLastCoordination += DeltaTime;
    
    if (TimeSinceLastCoordination >= 1.0f)
    {
        CoordinatePlans();
        SynchronizeWorldStates();
        TimeSinceLastCoordination = 0.0f;
    }
}
