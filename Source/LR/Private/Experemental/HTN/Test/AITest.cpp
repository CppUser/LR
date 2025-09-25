// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/Test/AITest.h"

float UAITask_MoveToLocation::CalculateUtility_Implementation(UHTNWorldState* WorldState) const
{
	return Super::CalculateUtility_Implementation(WorldState);
}

void UAITask_MoveToLocation::OnExecuteAction_Implementation(UHTNWorldState* WorldState)
{
	Super::OnExecuteAction_Implementation(WorldState);
}

void UAITask_MoveToLocation::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);
}

////////////////////////////////////////////////
///
////////////////////////////////////////////////
UAITask_Combat::UAITask_Combat()
{
	TaskName = TEXT("Combat");
	Priority = 10;
    
	SetupCombatMethods();
}

FHTNMethod UAITask_Combat::SelectBestMethod_Implementation(const TArray<FHTNMethod>& ValidMethods,
	UHTNWorldState* WorldState) const
{
	return Super::SelectBestMethod_Implementation(ValidMethods, WorldState);
}

void UAITask_Combat::SetupCombatMethods()
{
}
