// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "UObject/WeakInterfacePtr.h"
#include "LRAsyncAction_ObserveTeamColors.generated.h"

class ILRTeamAgentInterface;
class ULRTeamDisplayAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamColorObservedAsyncDelegate, bool, bTeamSet, int32, TeamId, const ULRTeamDisplayAsset*, DisplayAsset);


UCLASS()
class LR_API ULRAsyncAction_ObserveTeamColors : public UCancellableAsyncAction
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", Keywords="Watch"))
	static ULRAsyncAction_ObserveTeamColors* ObserveTeamColors(UObject* TeamAgent);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
private:
	void BroadcastChange(int32 NewTeam, const ULRTeamDisplayAsset* DisplayAsset);

	UFUNCTION()
	void OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	UFUNCTION()
	void OnDisplayAssetChanged(const ULRTeamDisplayAsset* DisplayAsset);
public:
	UPROPERTY(BlueprintAssignable)
	FTeamColorObservedAsyncDelegate OnTeamChanged;
private:

	TWeakInterfacePtr<ILRTeamAgentInterface> TeamInterfacePtr;
	TWeakObjectPtr<UObject> TeamInterfaceObj;

	int32 LastBroadcastTeamId = INDEX_NONE;
};
