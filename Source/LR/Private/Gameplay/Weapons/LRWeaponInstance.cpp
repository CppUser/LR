// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/LRWeaponInstance.h"

ULRWeaponInstance::ULRWeaponInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULRWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	UWorld* World = GetWorld();
	check(World);
	TimeLastEquipped = World->GetTimeSeconds();
	
}

void ULRWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

float ULRWeaponInstance::GetTimeSinceLastInteractedWith() const
{
	UWorld* World = GetWorld();
	check(World);
	const double WorldTime = World->GetTimeSeconds();

	double Result = WorldTime - TimeLastEquipped;
	
	return Result;
}

TSubclassOf<UAnimInstance> ULRWeaponInstance::PickBestAnimLayer(bool bEquipped,
	const FGameplayTagContainer& CosmeticTags) const
{
	//TODO: implement me
	return nullptr;
}
