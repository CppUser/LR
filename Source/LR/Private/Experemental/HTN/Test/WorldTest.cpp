// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/Test/WorldTest.h"

void UWorldTest::TriggerWorldEvent(const FWorldEvent& Event)
{
}

TArray<UHTNTask*> UWorldTest::GenerateResponseTasks(const FWorldEvent& Event)
{
	//TODO: Implement me
	return TArray<UHTNTask*>();
}

void UWorldTest::Execute_Implementation(UHTNWorldState* WorldState)
{
	Super::Execute_Implementation(WorldState);
}
