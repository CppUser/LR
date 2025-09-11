// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "LRGATargetActor_Interact.generated.h"

UCLASS(Blueprintable)
class AGameplayAbilityTargetActor_Interact : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

};
