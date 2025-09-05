// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Gameplay/GameMode/LRGameState.h"

#include "Gameplay/Experience/LRExperienceManagerComponent.h"



ALRGameState::ALRGameState(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ExperienceManagerComponent = CreateDefaultSubobject<ULRExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));

}

void ALRGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALRGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALRGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALRGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALRGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void ALRGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}

void ALRGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{
	//TODO: implement me for dungeon travel
	Super::SeamlessTravelTransitionCheckpoint(bToTransitionMap);
}
