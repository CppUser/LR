// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "LRAsyncAction_ObserveTeam.generated.h"

class ILRTeamAgentInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamObservedAsyncDelegate, bool, bTeamSet, int32, TeamId);

UCLASS()
class LR_API ULRAsyncAction_ObserveTeam : public UCancellableAsyncAction
{
	GENERATED_UCLASS_BODY()
public:

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	
	// Watches for team changes on the specified team agent
	//  - It will will fire once immediately to give the current team assignment
	//  - For anything that can ever belong to a team (implements ILyraTeamAgentInterface),
	//    it will also listen for team assignment changes in the future
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", Keywords="Watch"))
	static ULRAsyncAction_ObserveTeam* ObserveTeam(UObject* TeamAgent);
private:
	static ULRAsyncAction_ObserveTeam* InternalObserveTeamChanges(TScriptInterface<ILRTeamAgentInterface> TeamActor);

	UFUNCTION()
	void OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

public:
	UPROPERTY(BlueprintAssignable)
	FTeamObservedAsyncDelegate OnTeamChanged;

private:
	TWeakInterfacePtr<ILRTeamAgentInterface> TeamInterfacePtr;
};
