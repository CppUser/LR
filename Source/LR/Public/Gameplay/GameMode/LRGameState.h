// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "LRGameState.generated.h"

class ULRTeamDisplayAsset;
class ULRTeamCreationComponent;
class ULRExperienceManagerComponent;
/**
 * 
 */
UCLASS()
class LR_API ALRGameState : public AModularGameStateBase
{
	GENERATED_BODY()
public:
	ALRGameState(const FObjectInitializer& ObjInit = FObjectInitializer::Get());
	
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
	
private:
	UPROPERTY()
	TObjectPtr<ULRExperienceManagerComponent> ExperienceManagerComponent;

	UPROPERTY()
	TObjectPtr<ULRTeamCreationComponent> TeamCreationComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<uint8, TObjectPtr<ULRTeamDisplayAsset>> TeamsToCreate;
};
