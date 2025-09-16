// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LRActivatableWidget.h"
#include "LRHUDLayout.generated.h"


UCLASS(Abstract, BlueprintType, Blueprintable)
class LR_API ULRHUDLayout : public ULRActivatableWidget
{
	GENERATED_BODY()
public:
	ULRHUDLayout(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

};
