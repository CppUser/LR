// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "UObject/Object.h"
#include "LRUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class LR_API ULRUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
public:
	ULRUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();
	
	FTSTicker::FDelegateHandle TickHandle;
};

