// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LRWaitForInteractableTargets.h"
#include "Gameplay/Interaction/LRInteractionQuery.h"
#include "LRWaitForInteractableTargets_SLineTrace.generated.h"

/**
 * 
 */
UCLASS()
class LR_API ULRWaitForInteractableTargets_SLineTrace : public ULRWaitForInteractableTargets
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	/** Wait until we trace new set of interactables.  This task automatically loops. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static ULRWaitForInteractableTargets_SLineTrace* WaitForInteractableTargets_SingleLineTrace(UGameplayAbility* OwningAbility, FInteractionQuery InteractionQuery, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange = 100, float InteractionScanRate = 0.100, bool bShowDebug = false);

private:

	virtual void OnDestroy(bool AbilityEnded) override;

	void PerformTrace();

	UPROPERTY()
	FInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	float InteractionScanRange = 100;
	float InteractionScanRate = 0.100;
	bool bShowDebug = false;

	FTimerHandle TimerHandle;
};
