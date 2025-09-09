// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Weapons/LRWeaponInstance.h"
#include "LRRangeWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class LR_API ULRRangeWeaponInstance : public ULRWeaponInstance
{
	GENERATED_BODY()
public:
	ULRRangeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostLoad() override;
	void Tick(float DeltaSeconds);
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	void UpdateDebugVisualization();
#endif
	virtual void OnEquipped();
	virtual void OnUnequipped();

private:
	double TimeLastFired = 0.0;
};
