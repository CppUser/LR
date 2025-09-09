// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/Range/LRRangeWeaponInstance.h"

ULRRangeWeaponInstance::ULRRangeWeaponInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULRRangeWeaponInstance::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

#if WITH_EDITOR
void ULRRangeWeaponInstance::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateDebugVisualization();
}

void ULRRangeWeaponInstance::UpdateDebugVisualization()
{
	//TODO:
	// ComputeHeatRange(/*out*/ Debug_MinHeat, /*out*/ Debug_MaxHeat);
	// ComputeSpreadRange(/*out*/ Debug_MinSpreadAngle, /*out*/ Debug_MaxSpreadAngle);
	// Debug_CurrentHeat = CurrentHeat;
	// Debug_CurrentSpreadAngle = CurrentSpreadAngle;
	// Debug_CurrentSpreadAngleMultiplier = CurrentSpreadAngleMultiplier;
}
#endif
void ULRRangeWeaponInstance::Tick(float DeltaSeconds)
{
}

void ULRRangeWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
}

void ULRRangeWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}


