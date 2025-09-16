// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LRGameViewportClient.h"

ULRGameViewportClient::ULRGameViewportClient()
: Super(FObjectInitializer::Get())
{
}

void ULRGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
	bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
}
