// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "LRScanForInteractables.generated.h"

class ILRInteractableTarget;
struct FLRInteractionOption;
enum class EInteractionMethod : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractablesFound, const TArray<FLRInteractionOption>&, Options);

UCLASS()
class LR_API ULRScanForInteractables : public UAbilityTask
{
	GENERATED_BODY()
public:
	ULRScanForInteractables(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static ULRScanForInteractables* ScanForInteractables(
		     UGameplayAbility* OwningAbility,
			 float InScanRange,
	         float InScanRate,
	         float InScanAngle,
	         bool bInRequireLineOfSight,
	         bool bInScanInCone,
	         EInteractionMethod InMethod
	);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FOnInteractablesFound OnInteractablesFound;

private:
	void PerformScan();
	void ScanProximity();
	void ScanLineTrace();
	void ScanMouseOver();

	bool CheckLineOfSight(const AActor* Target) const;
	bool IsInScanArea(const AActor* Target) const;
    
	void ProcessInteractableTargets(const TArray<TScriptInterface<ILRInteractableTarget>>& Targets);
	void UpdateCachedOptions(const TArray<FLRInteractionOption>& NewOptions);

private:
	float ScanRange;
	float ScanRate;
	float ScanAngle;
	bool bRequireLineOfSight;
	bool bScanInCone;
	EInteractionMethod InteractionMethod;

	FTimerHandle ScanTimerHandle;
	TArray<FLRInteractionOption> CachedOptions;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> GrantedAbilityCache;
};


