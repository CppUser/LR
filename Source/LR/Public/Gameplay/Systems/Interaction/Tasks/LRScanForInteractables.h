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
	         EInteractionMethod InMethod,
	         bool bShowDebugInfo = false
	);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnInteractablesFound OnInteractablesFound;

private:
	void PerformScan();
	void GatherProximityTargets(TArray<TScriptInterface<ILRInteractableTarget>>& OutTargets);
	void FilterTargetsByMethod(TArray<TScriptInterface<ILRInteractableTarget>>& Targets);
	void FilterTargetsByCone(TArray<TScriptInterface<ILRInteractableTarget>>& Targets);
	void ScanProximity();
	void ScanLineTrace();
	void ScanMouseOver();

	bool CheckLineOfSight(const AActor* Target) const;
	bool IsInCone(const AActor* Target) const;
	bool IsLookingAtTarget(const AActor* Target) const;
	
	void ProcessInteractableTargets(const TArray<TScriptInterface<ILRInteractableTarget>>& Targets);
	void UpdateCachedOptions(const TArray<FLRInteractionOption>& NewOptions);

	void DrawDebugVisualization();
private:
	float ScanRange;
	float ScanRate;
	float ScanAngle;
	bool bRequireLineOfSight;
	bool bScanInCone;
	EInteractionMethod InteractionMethod;
	bool bShowDebugInfo;

	FTimerHandle ScanTimerHandle;
	TArray<FLRInteractionOption> CachedOptions;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> GrantedAbilityCache;

	FCollisionProfileName TraceProfile;
	bool bTraceAffectsAimPitch = true;
};


