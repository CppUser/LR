// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Gameplay/GameMode/LRGameInstance.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Utils/LRGameplayTags.h"

ULRGameInstance::ULRGameInstance(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}

void ULRGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		
		ComponentManager->RegisterInitState(LRGameplayTags::InitState_Spawned,false,FGameplayTag());
		ComponentManager->RegisterInitState(LRGameplayTags::InitState_DataAvailable,false,LRGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(LRGameplayTags::InitState_DataInitialized,false,LRGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(LRGameplayTags::InitState_GameplayReady,false,LRGameplayTags::InitState_DataInitialized);
	}
}

void ULRGameInstance::Shutdown()
{
	Super::Shutdown();
}
