// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/HTNPlanner.h"

#include "Experemental/HTN/HTNCompoundTask.h"
#include "Experemental/HTN/HTNExecutor.h"
#include "Experemental/HTN/HTNWorldState.h"

FHTNPlan UHTNPlanner::CreatePlan(UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	FHTNPlan Plan;
	Plan.bIsValid = false;
    
	if (!RootTask || !InitialState)
	{
		UE_LOG(LogHTN, Error, TEXT("CreatePlan: Invalid input"));
		return Plan;
	}
    
	UHTNWorldState* WorkingState = InitialState->Clone();
    
	FDateTime StartTime = FDateTime::Now();
	bool bSuccess = DecomposeTask(RootTask, WorkingState, Plan, 0);
    
	float ElapsedSeconds = (FDateTime::Now() - StartTime).GetTotalSeconds();
	if (ElapsedSeconds > PlanTimeout)
	{
		UE_LOG(LogHTN, Warning, TEXT("Planning timeout exceeded"));
		if (!bAllowPartialPlans)
		{
			Plan.Clear();
			return Plan;
		}
	}
    
	if (bSuccess || (bAllowPartialPlans && Plan.Steps.Num() > 0))
	{
		Plan.bIsValid = true;
		Plan.TotalUtility = EvaluatePlanUtility(Plan, InitialState);
	}
    
	UE_LOG(LogHTN, Log, TEXT("Created plan with %d steps, utility: %f"), 
		   Plan.Steps.Num(), Plan.TotalUtility);
    
	return Plan;
}

FHTNPlan UHTNPlanner::ReplanFromStep(const FHTNPlan& CurrentPlan, int32 StepIndex, UHTNWorldState* CurrentState)
{
	FHTNPlan NewPlan;
	NewPlan.bIsValid = false;
    
	if (!CurrentState || StepIndex >= CurrentPlan.Steps.Num())
	{
		return NewPlan;
	}
    
	for (int32 i = 0; i < StepIndex; i++)
	{
		NewPlan.Steps.Add(CurrentPlan.Steps[i]);
		NewPlan.TotalCost += CurrentPlan.Steps[i].EstimatedCost;
	}
    
	if (StepIndex < CurrentPlan.Steps.Num())
	{
		UHTNTask* RemainingTask = CurrentPlan.Steps[StepIndex].Task;
		if (RemainingTask)
		{
			UHTNWorldState* WorkingState = CurrentState->Clone();
			bool bSuccess = DecomposeTask(RemainingTask, WorkingState, NewPlan, StepIndex);
            
			if (bSuccess || (bAllowPartialPlans && NewPlan.Steps.Num() > StepIndex))
			{
				NewPlan.bIsValid = true;
				NewPlan.TotalUtility = EvaluatePlanUtility(NewPlan, CurrentState);
			}
		}
	}
    
	return NewPlan;
}

bool UHTNPlanner::ValidatePlan(const FHTNPlan& Plan, UHTNWorldState* CurrentState) const
{
	if (!Plan.bIsValid || !CurrentState) return false;
    
	UHTNWorldState* SimulatedState = CurrentState->Clone();
    
	for (const FHTNPlanStep& Step : Plan.Steps)
	{
		if (!Step.Task) return false;
        
		if (!Step.Task->CanExecute(SimulatedState))
		{
			return false;
		}
        
		for (const FHTNEffect& Effect : Step.Task->Effects)
		{
			SimulatedState->ApplyEffect(Effect);
		}
	}
    
	return true;
}

bool UHTNPlanner::DecomposeTask(UHTNTask* Task, UHTNWorldState* WorldState, FHTNPlan& OutPlan, int32 CurrentDepth)
{
	if (!Task || !WorldState) return false;
    
    if (CurrentDepth > MaxPlanDepth)
    {
        UE_LOG(LogHTN, Warning, TEXT("Max plan depth reached"));
        return false;
    }
    
    if (!Task->CanExecute(WorldState))
    {
        UE_LOG(LogHTN, Verbose, TEXT("Task %s cannot execute"), *Task->TaskName.ToString());
        return false;
    }
    
    if (UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(Task))
    {
        TArray<FHTNMethod> ValidMethods = CompoundTask->GetValidMethods(WorldState);
        
        if (ValidMethods.Num() == 0)
        {
            UE_LOG(LogHTN, Verbose, TEXT("No valid methods for compound task %s"), 
                   *Task->TaskName.ToString());
            return false;
        }
        
        FHTNMethod SelectedMethod;
        
        switch (PlannerMode)
        {
        case EHTNPlannerMode::Dynamic:
        case EHTNPlannerMode::UtilityDriven:
            SelectedMethod = CompoundTask->SelectBestMethod(ValidMethods, WorldState);
            break;
            
        case EHTNPlannerMode::Probabilistic:
            {
                // Probabilistic selection
                float TotalScore = 0.0f;
                for (const FHTNMethod& Method : ValidMethods)
                {
                    TotalScore += Method.PreferenceScore;
                }
                
                float Random = FMath::FRandRange(0.0f, TotalScore);
                float Current = 0.0f;
                
                for (const FHTNMethod& Method : ValidMethods)
                {
                    Current += Method.PreferenceScore;
                    if (Random <= Current)
                    {
                        SelectedMethod = Method;
                        break;
                    }
                }
            }
            break;
            
        default:
            SelectedMethod = ValidMethods[0];
            break;
        }
        
        UHTNWorldState* TempWorldState = WorldState->Clone();
        bool bAllSubtasksSucceeded = true;
        
        for (TSubclassOf<UHTNTask> SubtaskClass : SelectedMethod.Subtasks)
        {
            if (!SubtaskClass) continue;
            
            UHTNTask* Subtask = NewObject<UHTNTask>(this, SubtaskClass);
            
            if (!DecomposeTask(Subtask, TempWorldState, OutPlan, CurrentDepth + 1))
            {
                bAllSubtasksSucceeded = false;
                if (!bAllowPartialPlans) return false;
            }
            
            for (const FHTNEffect& Effect : Subtask->Effects)
            {
                TempWorldState->ApplyEffect(Effect);
            }
        }
        
        return bAllSubtasksSucceeded || (bAllowPartialPlans && OutPlan.Steps.Num() > 0);
    }
    else // Primitive task
    {
        FHTNPlanStep Step;
        Step.Task = Task;
        Step.Depth = CurrentDepth;
        Step.EstimatedCost = 1.0f;
        Step.UtilityScore = Task->CalculateUtility(WorldState);
        
        OutPlan.Steps.Add(Step);
        OutPlan.TotalCost += Step.EstimatedCost;
        OutPlan.TotalUtility += Step.UtilityScore;
        
        return true;
    }
}

float UHTNPlanner::EvaluatePlanUtility(const FHTNPlan& Plan, UHTNWorldState* WorldState) const
{
	float TotalUtility = 0.0f;
	float DiscountFactor = 1.0f;
    
	UHTNWorldState* SimulatedState = WorldState->Clone();
    
	for (const FHTNPlanStep& Step : Plan.Steps)
	{
		if (!Step.Task) continue;
        
		float StepUtility = Step.Task->CalculateUtility(SimulatedState);
		TotalUtility += StepUtility * DiscountFactor;
        
		DiscountFactor *= 0.95f;
        
		for (const FHTNEffect& Effect : Step.Task->Effects)
		{
			SimulatedState->ApplyEffect(Effect);
		}
	}
    
	return TotalUtility;
}

FHTNPlanningTask::FHTNPlanningTask(UHTNTask* InRootTask, UHTNWorldState* InWorldState, UHTNPlanner* InPlanner)
: RootTask(InRootTask)
	, WorldState(InWorldState)
	, Planner(InPlanner)
{
}

void FHTNPlanningTask::DoWork()
{
	if (Planner && RootTask && WorldState)
	{
		ResultPlan = Planner->CreatePlan(RootTask, WorldState);
	}
}

/////////////////////////////////////////////////////
///            UHTNAsyncPlanner
////////////////////////////////////////////////////
void UHTNAsyncPlanner::CreatePlanAsync(UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	if (!RootTask || !InitialState)
	{
		UE_LOG(LogHTN, Error, TEXT("CreatePlanAsync: Invalid input"));
		return;
	}
    
	if (IsPlanningInProgress())
	{
		UE_LOG(LogHTN, Warning, TEXT("Planning already in progress"));
		return;
	}
    
	CurrentPlanningTask = MakeUnique<FAsyncTask<FHTNPlanningTask>>(
		RootTask, InitialState, this);
    
	CurrentPlanningTask->StartBackgroundTask();
    
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PlanningCheckTimer,
			this,
			&UHTNAsyncPlanner::OnPlanningComplete,
			0.1f,
			true
		);
	}
}

bool UHTNAsyncPlanner::IsPlanningInProgress() const
{
	return CurrentPlanningTask && !CurrentPlanningTask->IsDone();
}

void UHTNAsyncPlanner::CancelAsyncPlanning()
{
	if (CurrentPlanningTask)
	{
		CurrentPlanningTask->Cancel();
		CurrentPlanningTask.Reset();
	}
    
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PlanningCheckTimer);
	}
}

void UHTNAsyncPlanner::OnPlanningComplete()
{
	if (CurrentPlanningTask && CurrentPlanningTask->IsDone())
	{
		FHTNPlan ResultPlan = CurrentPlanningTask->GetTask().ResultPlan;
        
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PlanningCheckTimer);
		}
        
		OnAsyncPlanComplete.Broadcast(ResultPlan);
		CurrentPlanningTask.Reset();
	}
}
/////////////////////////////////////////////////////
///            UHTNReactiveLayer
////////////////////////////////////////////////////
void UHTNReactiveLayer::CheckReactiveRules(UHTNWorldState* WorldState)
{
	if (!WorldState || !LinkedExecutor) return;
    
	FHTNReactiveRule* TriggeredRule = nullptr;
	float HighestPriority = -1.0f;
    
	for (FHTNReactiveRule& Rule : ReactiveRules)
	{
		bool bAllConditionsMet = true;
        
		for (const FHTNCondition& Condition : Rule.TriggerConditions)
		{
			if (!Condition.Evaluate(WorldState))
			{
				bAllConditionsMet = false;
				break;
			}
		}
        
		if (bAllConditionsMet && Rule.Priority > HighestPriority)
		{
			TriggeredRule = &Rule;
			HighestPriority = Rule.Priority;
		}
	}
    
	if (TriggeredRule && TriggeredRule->RuleName != LastTriggeredRule)
	{
		LastTriggeredRule = TriggeredRule->RuleName;
        
		UE_LOG(LogHTN, Log, TEXT("Reactive rule triggered: %s"), *TriggeredRule->RuleName.ToString());
        
		if (TriggeredRule->bInterruptCurrent && TriggeredRule->ReactiveTask)
		{
			UHTNTask* ReactiveTask = NewObject<UHTNTask>(this, TriggeredRule->ReactiveTask);
			ReactiveTask->Priority = TriggeredRule->Priority;
            
			LinkedExecutor->InterruptCurrent(ReactiveTask);
		}
	}
	else if (!TriggeredRule)
	{
		LastTriggeredRule = NAME_None;
	}
}

void UHTNReactiveLayer::AddReactiveRule(const FHTNReactiveRule& Rule)
{
	ReactiveRules.Add(Rule);
    
	ReactiveRules.Sort([](const FHTNReactiveRule& A, const FHTNReactiveRule& B)
	{
		return A.Priority > B.Priority;
	});
}

void UHTNReactiveLayer::RemoveReactiveRule(FName RuleName)
{
	ReactiveRules.RemoveAll([RuleName](const FHTNReactiveRule& Rule)
	{
		return Rule.RuleName == RuleName;
	});
}

void UHTNReactiveLayer::Tick(float DeltaTime)
{
	if (!LinkedExecutor || !LinkedExecutor->WorldState) return;
    
	TimeSinceLastCheck += DeltaTime;
    
	if (TimeSinceLastCheck >= ReactiveCheckInterval)
	{
		CheckReactiveRules(LinkedExecutor->WorldState);
		TimeSinceLastCheck = 0.0f;
	}
}



/////////////////////////////////////////////////////
///            UHTNPartialOrderPlanner
////////////////////////////////////////////////////
bool FHTNPartialPlan::IsValid() const
{
	if (Tasks.Num() == 0) return false;
    
	TArray<int32> Visited;
	TArray<int32> RecStack;
    
	for (int32 i = 0; i < Tasks.Num(); i++)
	{
		if (!Visited.Contains(i))
		{
			TArray<int32> Stack;
			Stack.Push(i);
            
			while (Stack.Num() > 0)
			{
				int32 Current = Stack.Last();
                
				if (!Visited.Contains(Current))
				{
					Visited.Add(Current);
					RecStack.Add(Current);

					//TODO: Fix
					// const TArray<int32>* Deps = Dependencies.Find(Current);
					// if (Deps)
					// {
					// 	for (int32 Dep : *Deps)
					// 	{
					// 		if (!Visited.Contains(Dep))
					// 		{
					// 			Stack.Push(Dep);
					// 		}
					// 		else if (RecStack.Contains(Dep))
					// 		{
					// 			return false;
					// 		}
					// 	}
					// }
				}
				else
				{
					Stack.Pop();
					RecStack.Remove(Current);
				}
			}
		}
	}
    
	return true;
}

TArray<int32> FHTNPartialPlan::GetExecutableTaskIndices() const
{
	TArray<int32> Executable;
    
	for (int32 i = 0; i < Tasks.Num(); i++)
	{
		bool bCanExecute = true;
        
		for (const FHTNOrderingConstraint& Constraint : OrderingConstraints)
		{
			if (Constraint.AfterTaskIndex == i)
			{
				// This task must come after BeforeTaskIndex
				// Check if BeforeTaskIndex is completed (not in list)
				if (Tasks.IsValidIndex(Constraint.BeforeTaskIndex) && 
					Tasks[Constraint.BeforeTaskIndex] != nullptr)
				{
					bCanExecute = false;
					break;
				}
			}
		}
        
		if (bCanExecute && Tasks[i] != nullptr)
		{
			Executable.Add(i);
		}
	}
    
	return Executable;
}

FHTNPartialPlan UHTNPartialOrderPlanner::CreatePartialOrderPlan(UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	FHTNPartialPlan PartialPlan;
    
	if (!RootTask || !InitialState) return PartialPlan;
    
	FHTNPlan RegularPlan = CreatePlan(RootTask, InitialState);
    
	if (!RegularPlan.bIsValid) return PartialPlan;
    
	for (const FHTNPlanStep& Step : RegularPlan.Steps)
	{
		PartialPlan.Tasks.Add(Step.Task);
	}
    
	AnalyzeTaskDependencies(PartialPlan.Tasks, PartialPlan);
    
	return PartialPlan;
}

TArray<UHTNTask*> UHTNPartialOrderPlanner::GetParallelExecutableTasks(const FHTNPartialPlan& Plan)
{
	TArray<UHTNTask*> ParallelTasks;
    
	TArray<int32> ExecutableIndices = Plan.GetExecutableTaskIndices();
    
	for (int32 Index : ExecutableIndices)
	{
		if (Plan.Tasks.IsValidIndex(Index))
		{
			ParallelTasks.Add(Plan.Tasks[Index]);
		}
	}
    
	return ParallelTasks;
}

void UHTNPartialOrderPlanner::UpdatePlanConstraints(FHTNPartialPlan& Plan, int32 CompletedTaskIndex)
{
	if (!Plan.Tasks.IsValidIndex(CompletedTaskIndex)) return;
    
	Plan.Tasks[CompletedTaskIndex] = nullptr;
    
	Plan.OrderingConstraints.RemoveAll([CompletedTaskIndex](const FHTNOrderingConstraint& Constraint)
	{
		return Constraint.BeforeTaskIndex == CompletedTaskIndex;
	});
    
	Plan.Dependencies.Remove(CompletedTaskIndex);
    
	for (auto& DepPair : Plan.Dependencies)
	{
		//TODO:Fix DepPair.Value.Remove(CompletedTaskIndex);
	}
}

void UHTNPartialOrderPlanner::AnalyzeTaskDependencies(const TArray<UHTNTask*>& Tasks, FHTNPartialPlan& OutPlan)
{
	for (int32 i = 0; i < Tasks.Num(); i++)
	{
		for (int32 j = i + 1; j < Tasks.Num(); j++)
		{
			if (!CanExecuteInParallel(Tasks[i], Tasks[j]))
			{
				FHTNOrderingConstraint Constraint;
				Constraint.BeforeTaskIndex = i;
				Constraint.AfterTaskIndex = j;
				Constraint.bStrict = true;
				OutPlan.OrderingConstraints.Add(Constraint);
                
				if (!OutPlan.Dependencies.Contains(j))
				{
					//TODO:Fix OutPlan.Dependencies.Add(j, TArray<int32>());
				}
				//TODO:Fix OutPlan.Dependencies[j].Add(i);
			}
		}
	}
}

bool UHTNPartialOrderPlanner::CanExecuteInParallel(UHTNTask* Task1, UHTNTask* Task2)
{
	if (!Task1 || !Task2) return false;
    
	for (const FHTNEffect& Effect1 : Task1->Effects)
	{
		for (const FHTNEffect& Effect2 : Task2->Effects)
		{
			if (Effect1.PropertyName == Effect2.PropertyName)
			{
				return false;
			}
		}
        
		for (const FHTNCondition& Condition2 : Task2->Preconditions)
		{
			if (Condition2.PropertyName == Effect1.PropertyName)
			{
				return false;
			}
		}
	}
    
	for (const FHTNEffect& Effect2 : Task2->Effects)
	{
		for (const FHTNCondition& Condition1 : Task1->Preconditions)
		{
			if (Condition1.PropertyName == Effect2.PropertyName)
			{
				return false;
			}
		}
	}
    
	return true;
}

/////////////////////////////////////////////////////
///           UHTNProbabilisticTask
////////////////////////////////////////////////////
FHTNProbabilisticOutcome UHTNProbabilisticTask::SampleOutcome() const
{
	if (PossibleOutcomes.Num() == 0)
	{
		FHTNProbabilisticOutcome DefaultOutcome;
		DefaultOutcome.Effects = Effects;
		DefaultOutcome.Probability = 1.0f;
		return DefaultOutcome;
	}
    
	float Random = FMath::FRand();
	float CumulativeProbability = 0.0f;
    
	for (const FHTNProbabilisticOutcome& Outcome : PossibleOutcomes)
	{
		CumulativeProbability += Outcome.Probability;
		if (Random <= CumulativeProbability)
		{
			return Outcome;
		}
	}
    
	return PossibleOutcomes.Last();
}

float UHTNProbabilisticTask::CalculateExpectedUtility(UHTNWorldState* WorldState) const
{
	if (!WorldState) return 0.0f;
    
	float ExpectedUtility = 0.0f;
    
	for (const FHTNProbabilisticOutcome& Outcome : PossibleOutcomes)
	{
		UHTNWorldState* TempState = WorldState->Clone();
        
		for (const FHTNEffect& Effect : Outcome.Effects)
		{
			TempState->ApplyEffect(Effect);
		}
        
		float OutcomeUtility = UtilityScore + Outcome.UtilityModifier;
		ExpectedUtility += OutcomeUtility * Outcome.Probability;
	}
    
	return ExpectedUtility;
}

void UHTNProbabilisticTask::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);

	FHTNProbabilisticOutcome SelectedOutcome = SampleOutcome();
    
	for (const FHTNEffect& Effect : SelectedOutcome.Effects)
	{
		WorldState->ApplyEffect(Effect);
	}
    
	UtilityScore += SelectedOutcome.UtilityModifier;
}

/////////////////////////////////////////////////////
///           UHTNMonteCarloPlanner
////////////////////////////////////////////////////
FHTNPlan UHTNMonteCarloPlanner::CreateMonteCarloP(UHTNTask* RootTask, UHTNWorldState* InitialState)
{
	if (!RootTask || !InitialState) return FHTNPlan();
    
	TArray<FHTNPlan> CandidatePlans;
    
	for (int32 i = 0; i < FMath::Min(10, NumSimulations / 10); i++)
	{
		EHTNPlannerMode OldMode = PlannerMode;
		PlannerMode = EHTNPlannerMode::Probabilistic;
        
		FHTNPlan CandidatePlan = CreatePlan(RootTask, InitialState);
        
		PlannerMode = OldMode;
        
		if (CandidatePlan.bIsValid)
		{
			CandidatePlans.Add(CandidatePlan);
		}
	}
    
	if (CandidatePlans.Num() == 0)
	{
		return CreatePlan(RootTask, InitialState);
	}
    
	return SelectBestPlanFromSimulations(CandidatePlans);
}

float UHTNMonteCarloPlanner::SimulatePlan(const FHTNPlan& Plan, UHTNWorldState* InitialState)
{
	float TotalReward = 0.0f;
	float CurrentDiscount = 1.0f;
    
	UHTNWorldState* SimState = InitialState->Clone();
    
	for (const FHTNPlanStep& Step : Plan.Steps)
	{
		if (!Step.Task) continue;
        
		float StepReward = Step.UtilityScore;
        
		if (UHTNProbabilisticTask* ProbTask = Cast<UHTNProbabilisticTask>(Step.Task))
		{
			StepReward = ProbTask->CalculateExpectedUtility(SimState);
            
			FHTNProbabilisticOutcome Outcome = ProbTask->SampleOutcome();
			for (const FHTNEffect& Effect : Outcome.Effects)
			{
				SimState->ApplyEffect(Effect);
			}
		}
		else
		{
			for (const FHTNEffect& Effect : Step.Task->Effects)
			{
				SimState->ApplyEffect(Effect);
			}
		}
        
		TotalReward += StepReward * CurrentDiscount;
		CurrentDiscount *= DiscountFactor;
	}
    
	return TotalReward;
}

FHTNPlan UHTNMonteCarloPlanner::SelectBestPlanFromSimulations(const TArray<FHTNPlan>& CandidatePlans)
{
	if (CandidatePlans.Num() == 0) return FHTNPlan();
    
	FHTNPlan BestPlan = CandidatePlans[0];
	float BestAverageReward = -FLT_MAX;
    
	for (const FHTNPlan& Plan : CandidatePlans)
	{
		float TotalReward = 0.0f;
        
		for (int32 i = 0; i < NumSimulations; i++)
		{
			// Need initial state for simulation - assuming we have access to it
			// TODO: this should be passed in or stored
			UHTNWorldState* InitialState = NewObject<UHTNWorldState>();
			TotalReward += SimulatePlan(Plan, InitialState);
		}
        
		float AverageReward = TotalReward / NumSimulations;
        
		if (AverageReward > BestAverageReward)
		{
			BestAverageReward = AverageReward;
			BestPlan = Plan;
		}
	}
    
	BestPlan.TotalUtility = BestAverageReward;
	return BestPlan;
}

/////////////////////////////////////////////////////
///           UHTNEmergentBehaviorSystem
////////////////////////////////////////////////////
void UHTNEmergentBehaviorSystem::ObserveTaskSequence(const TArray<UHTNTask*>& ExecutedTasks, bool bSuccessful)
{
	if (ExecutedTasks.Num() < 2) return; // Need at least 2 tasks for a pattern
    
	TArray<FName> TaskNames;
	for (UHTNTask* Task : ExecutedTasks)
	{
		if (Task)
		{
			TaskNames.Add(Task->TaskName);
		}
	}
    
	ObservedSequences.Add(TaskNames);
    
	FString PatternKey = FString::JoinBy(TaskNames, TEXT("_"), 
		[](const FName& Name) { return Name.ToString(); });
    
	if (!PatternSuccessRates.Contains(PatternKey))
	{
		PatternSuccessRates.Add(PatternKey, 0.0f);
	}
    
	float& SuccessRate = PatternSuccessRates[PatternKey];
	SuccessRate = SuccessRate * (1.0f - PatternLearningRate) + 
				  (bSuccessful ? 1.0f : 0.0f) * PatternLearningRate;
    
	if (SuccessRate > 0.7f && TaskNames.Num() >= 3)
	{
		bool bPatternExists = false;
		for (const FHTNBehaviorPattern& Pattern : KnownPatterns)
		{
			if (Pattern.RequiredTasks == TaskNames)
			{
				bPatternExists = true;
				break;
			}
		}
        
		if (!bPatternExists)
		{
			FHTNBehaviorPattern NewPattern;
			NewPattern.PatternName = FName(*FString::Printf(TEXT("EmergentPattern_%d"), 
															 KnownPatterns.Num()));
			NewPattern.RequiredTasks = TaskNames;
			NewPattern.EmergenceThreshold = SuccessRate;
            
			KnownPatterns.Add(NewPattern);
            
			UE_LOG(LogHTN, Log, TEXT("Discovered emergent pattern: %s"), 
				   *NewPattern.PatternName.ToString());
		}
	}
}

TArray<FHTNBehaviorPattern> UHTNEmergentBehaviorSystem::DetectEmergentPatterns(const TArray<UHTNTask*>& TaskHistory)
{
	TArray<FHTNBehaviorPattern> DetectedPatterns;
    
	if (TaskHistory.Num() < 2) return DetectedPatterns;
    
	TArray<FName> HistoryNames;
	for (UHTNTask* Task : TaskHistory)
	{
		if (Task)
		{
			HistoryNames.Add(Task->TaskName);
		}
	}
    
	for (const FHTNBehaviorPattern& Pattern : KnownPatterns)
	{
		int32 PatternIndex = 0;
		int32 HistoryIndex = 0;
        
		while (HistoryIndex < HistoryNames.Num() && 
			   PatternIndex < Pattern.RequiredTasks.Num())
		{
			if (HistoryNames[HistoryIndex] == Pattern.RequiredTasks[PatternIndex])
			{
				PatternIndex++;
			}
			HistoryIndex++;
		}
        
		if (PatternIndex == Pattern.RequiredTasks.Num())
		{
			DetectedPatterns.Add(Pattern);
		}
	}
    
	return DetectedPatterns;
}

UHTNCompoundTask* UHTNEmergentBehaviorSystem::GenerateEmergentTask(const FHTNBehaviorPattern& Pattern)
{
	UHTNCompoundTask* EmergentTask = NewObject<UHTNCompoundTask>();
	EmergentTask->TaskName = Pattern.PatternName;
	EmergentTask->Priority = 5; // Medium priority for emergent behaviors
    
	FHTNMethod EmergentMethod;
	EmergentMethod.MethodName = FName(*FString::Printf(TEXT("%s_Method"), 
													   *Pattern.PatternName.ToString()));
	EmergentMethod.Preconditions = Pattern.ActivationConditions;
	EmergentMethod.PreferenceScore = Pattern.EmergenceThreshold;
    
	//TODO:  need to map task names back to task classes
	// This would require a registry or lookup system
	// For now, leave the subtasks empty
    
	EmergentTask->Methods.Add(EmergentMethod);
    
	return EmergentTask;
}