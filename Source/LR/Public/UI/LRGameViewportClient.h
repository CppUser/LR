// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonGameViewportClient.h"
#include "LRGameViewportClient.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LR_API ULRGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
public:
	ULRGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;

};
