// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LRInteractionOption.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LRInteractionStatics.generated.h"

class ILRInteractableTarget;
/**
 * 
 */
UCLASS()
class ULRInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	ULRInteractionStatics();

public:
	UFUNCTION(BlueprintCallable)
	static AActor* GetActorFromInteractableTarget(TScriptInterface<ILRInteractableTarget> InteractableTarget);

	UFUNCTION(BlueprintCallable)
	static void GetInteractableTargetsFromActor(AActor* Actor, TArray<TScriptInterface<ILRInteractableTarget>>& OutInteractableTargets);

	static void AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<ILRInteractableTarget>>& OutInteractableTargets);
	static void AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<ILRInteractableTarget>>& OutInteractableTargets);

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	static FText GetInteractionVerb(EInteractionType Type);

	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	static bool CheckAngleRequirement(const AActor* Interactor, const AActor* Target, float RequiredAngle);
};

