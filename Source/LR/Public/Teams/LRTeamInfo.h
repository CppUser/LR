// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Utils/LRGameplayTagStack.h"
#include "LRTeamInfo.generated.h"

class ULRTeamDisplayAsset;
class ULRTeamSubsystem;
/**
 * 
 */
UCLASS()
class LR_API ALRTeamInfo : public AInfo
{
	GENERATED_BODY()
public:
	ALRTeamInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	int32 GetTeamId() const { return TeamId; }
	void SetTeamId(int32 NewTeamId);

	ULRTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }
	void SetTeamDisplayAsset(TObjectPtr<ULRTeamDisplayAsset> NewDisplayAsset);
	
protected:
	virtual void RegisterWithTeamSubsystem(ULRTeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

public:
	UPROPERTY()
	FGameplayTagStackContainer TeamTags;

private:
	UPROPERTY()
	int32 TeamId;
	
	UPROPERTY()
	TObjectPtr<ULRTeamDisplayAsset> TeamDisplayAsset;
};
