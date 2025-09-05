// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "LRWorldSettings.generated.h"

class ULRExperience;
/**
 * 
 */
UCLASS()
class LR_API ALRWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	FPrimaryAssetId GetDefaultGameplayExperience() const;
protected:
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<ULRExperience> DefaultGameplayExperience;

};
