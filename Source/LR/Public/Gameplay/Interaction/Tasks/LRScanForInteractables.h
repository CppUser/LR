// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "LRScanForInteractables.generated.h"

enum class EInteractionMethod : uint8;
struct FInteractionOption;
class IInteractableTarget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractablesFound, const TArray<FInteractionOption>&, Options);


UCLASS()
class LR_API ULRScanForInteractables : public UAbilityTask
{
	GENERATED_BODY()
public:
	ULRScanForInteractables(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static ULRScanForInteractables* ScanForInteractables(
		UGameplayAbility* OwningAbility,
		float ScanRange,
		float ScanRate,
		float ScanAngle,
		bool bRequireLineOfSight,
		bool bScanInCone,
		EInteractionMethod Method
	);

	UPROPERTY(BlueprintAssignable)
	FOnInteractablesFound OnInteractablesFound;

private:
	void PerformScan();
	void ScanProximity();
	void ScanLineTrace();
	void ScanMouseOver();
    
	bool CheckLineOfSight(const AActor* Target) const;
	bool IsInScanArea(const AActor* Target) const;
    
	void ProcessInteractableTargets(const TArray<TScriptInterface<IInteractableTarget>>& Targets);
	void UpdateCachedOptions(const TArray<FInteractionOption>& NewOptions);

private:
	float ScanRange;
	float ScanRate;
	float ScanAngle;
	bool bRequireLineOfSight;
	bool bScanInCone;
	EInteractionMethod InteractionMethod;

	FTimerHandle ScanTimerHandle;
	TArray<FInteractionOption> CachedOptions;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> GrantedAbilityCache;
};
