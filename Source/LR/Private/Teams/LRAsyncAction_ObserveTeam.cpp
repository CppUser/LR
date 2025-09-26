// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/LRAsyncAction_ObserveTeam.h"

#include "Teams/ILRTeamAgentInterface.h"


ULRAsyncAction_ObserveTeam::ULRAsyncAction_ObserveTeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULRAsyncAction_ObserveTeam::Activate()
{
	bool bCouldSucceed = false;
	int32 CurrentTeamIndex = INDEX_NONE;

	if (ILRTeamAgentInterface* TeamInterface = TeamInterfacePtr.Get())
	{
		CurrentTeamIndex = GenericTeamIdToInteger(TeamInterface->GetGenericTeamId());

		TeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnWatchedAgentChangedTeam);

		bCouldSucceed = true;
	}

	// Broadcast once so users get the current state
	OnTeamChanged.Broadcast(CurrentTeamIndex != INDEX_NONE, CurrentTeamIndex);

	// We weren't able to bind to a delegate so we'll never get any additional updates
	if (!bCouldSucceed)
	{
		SetReadyToDestroy();
	}
}

void ULRAsyncAction_ObserveTeam::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();

	if (ILRTeamAgentInterface* TeamInterface = TeamInterfacePtr.Get())
	{
		TeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
	}
}

ULRAsyncAction_ObserveTeam* ULRAsyncAction_ObserveTeam::ObserveTeam(UObject* TeamAgent)
{
	return InternalObserveTeamChanges(TeamAgent);
}

ULRAsyncAction_ObserveTeam* ULRAsyncAction_ObserveTeam::InternalObserveTeamChanges(
	TScriptInterface<ILRTeamAgentInterface> TeamActor)
{
	ULRAsyncAction_ObserveTeam* Action = nullptr;

	if (TeamActor != nullptr)
	{
		Action = NewObject<ULRAsyncAction_ObserveTeam>();
		Action->TeamInterfacePtr = TeamActor;
		Action->RegisterWithGameInstance(TeamActor.GetObject());
	}

	return Action;
}

void ULRAsyncAction_ObserveTeam::OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	OnTeamChanged.Broadcast(NewTeam != INDEX_NONE, NewTeam);
}
