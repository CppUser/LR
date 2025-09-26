// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Player/LRLocalPlayer.h"

ULRLocalPlayer::ULRLocalPlayer()
{
}

void ULRLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();
}

void ULRLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

	OnPlayerControllerChanged(PlayerController);
}

bool ULRLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	OnPlayerControllerChanged(PlayerController);

	return bResult;
}

void ULRLocalPlayer::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	// Do nothing, we merely observe the team of our associated player controller
}

FGenericTeamId ULRLocalPlayer::GetGenericTeamId() const
{
	if (ILRTeamAgentInterface* ControllerAsTeamProvider = Cast<ILRTeamAgentInterface>(PlayerController))
	{
		return ControllerAsTeamProvider->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
}

FOnTeamIndexChangedDelegate* ULRLocalPlayer::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ULRLocalPlayer::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void ULRLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (ILRTeamAgentInterface* ControllerAsTeamProvider = Cast<ILRTeamAgentInterface>(LastBoundPC.Get()))
	{
		OldTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	// Grab the current team ID and listen for future changes
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (ILRTeamAgentInterface* ControllerAsTeamProvider = Cast<ILRTeamAgentInterface>(NewController))
	{
		NewTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
		LastBoundPC = NewController;
	}

	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
}
