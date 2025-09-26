// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/ILRTeamAgentInterface.h"

ULRTeamAgentInterface::ULRTeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ILRTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<ILRTeamAgentInterface> This,
	FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID); 
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}
