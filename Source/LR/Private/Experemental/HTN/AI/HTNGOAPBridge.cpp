// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/AI/HTNGOAPBridge.h"

#include "Experemental/HTN/HTNWorldState.h"

TArray<FGOAPAction> UHTNGOAPBridge::PlanGOAP(UHTNWorldState* CurrentState)
{
	TArray<FGOAPAction> Plan;
    
    if (!CurrentState) return Plan;
    
    // A* search for GOAP planning
    struct FGOAPNode
    {
        UHTNWorldState* State;
        FGOAPAction Action;
        float Cost;
        float Heuristic;
        TSharedPtr<FGOAPNode> Parent;
        
        float GetFCost() const { return Cost + Heuristic; }
    };
    
    // Create goal state
    UHTNWorldState* GoalState = CurrentState->Clone();
    for (const FHTNCondition& Condition : Goal.DesiredState)
    {
        FHTNWorldProperty Property;
        Property.PropertyName = Condition.PropertyName;
        Property.Value = Condition.ExpectedValue;
        Property.NumericValue = Condition.NumericThreshold;
        GoalState->SetProperty(Condition.PropertyName, Property);
    }
    
    // Initialize open and closed sets
    TArray<TSharedPtr<FGOAPNode>> OpenSet;
    TArray<TSharedPtr<FGOAPNode>> ClosedSet;
    
    // Create start node
    TSharedPtr<FGOAPNode> StartNode = MakeShareable(new FGOAPNode());
    StartNode->State = CurrentState->Clone();
    StartNode->Cost = 0.0f;
    StartNode->Heuristic = CurrentState->CalculateDifference(GoalState);
    
    OpenSet.Add(StartNode);
    
    // A* search loop
    while (OpenSet.Num() > 0)
    {
        // Find node with lowest F cost
        TSharedPtr<FGOAPNode> CurrentNode = OpenSet[0];
        int32 CurrentIndex = 0;
        
        for (int32 i = 1; i < OpenSet.Num(); i++)
        {
            if (OpenSet[i]->GetFCost() < CurrentNode->GetFCost())
            {
                CurrentNode = OpenSet[i];
                CurrentIndex = i;
            }
        }
        
        // Move current node from open to closed
        OpenSet.RemoveAt(CurrentIndex);
        ClosedSet.Add(CurrentNode);
        
        // Check if we reached goal
        bool bGoalReached = true;
        for (const FHTNCondition& Condition : Goal.DesiredState)
        {
            if (!Condition.Evaluate(CurrentNode->State))
            {
                bGoalReached = false;
                break;
            }
        }
        
        if (bGoalReached)
        {
            // Reconstruct path
            TSharedPtr<FGOAPNode> PathNode = CurrentNode;
            while (PathNode && PathNode->Parent)
            {
                Plan.Insert(PathNode->Action, 0);
                PathNode = PathNode->Parent;
            }
            break;
        }
        
        // Explore available actions
        for (const FGOAPAction& Action : AvailableActions)
        {
            // Check if action is applicable
            bool bCanApply = true;
            for (const FHTNCondition& Precondition : Action.Preconditions)
            {
                if (!Precondition.Evaluate(CurrentNode->State))
                {
                    bCanApply = false;
                    break;
                }
            }
            
            if (!bCanApply) continue;
            
            // Create new node
            TSharedPtr<FGOAPNode> NewNode = MakeShareable(new FGOAPNode());
            NewNode->State = CurrentNode->State->Clone();
            NewNode->Action = Action;
            NewNode->Parent = CurrentNode;
            NewNode->Cost = CurrentNode->Cost + Action.Cost;
            
            // Apply action effects
            for (const FHTNEffect& Effect : Action.Effects)
            {
                NewNode->State->ApplyEffect(Effect);
            }
            
            NewNode->Heuristic = NewNode->State->CalculateDifference(GoalState);
            
            // Check if node is in closed set
            bool bInClosedSet = false;
            for (const TSharedPtr<FGOAPNode>& ClosedNode : ClosedSet)
            {
                if (ClosedNode->State->CalculateDifference(NewNode->State) < 0.01f)
                {
                    bInClosedSet = true;
                    break;
                }
            }
            
            if (bInClosedSet) continue;
            
            // Check if node is in open set
            bool bInOpenSet = false;
            for (const TSharedPtr<FGOAPNode>& OpenNode : OpenSet)
            {
                if (OpenNode->State->CalculateDifference(NewNode->State) < 0.01f)
                {
                    if (NewNode->Cost < OpenNode->Cost)
                    {
                        OpenNode->Cost = NewNode->Cost;
                        OpenNode->Parent = CurrentNode;
                    }
                    bInOpenSet = true;
                    break;
                }
            }
            
            if (!bInOpenSet)
            {
                OpenSet.Add(NewNode);
            }
        }
    }
    
    return Plan;
}

void UHTNGOAPBridge::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

    CurrentGOAPPlan = PlanGOAP(WorldState);
    CurrentActionIndex = 0;
    
    UE_LOG(LogHTN, Log, TEXT("GOAP planning complete with %d actions"), CurrentGOAPPlan.Num());
    
    if (CurrentGOAPPlan.Num() == 0)
    {
        SetStatus(EHTNTaskStatus::Failed);
    }
}
