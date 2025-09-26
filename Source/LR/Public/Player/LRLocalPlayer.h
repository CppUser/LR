// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "Teams/ILRTeamAgentInterface.h"
#include "LRLocalPlayer.generated.h"

//TODO: Set input mapping context Here ? Thru settings ?
UCLASS()
class LR_API ULRLocalPlayer : public UCommonLocalPlayer, public ILRTeamAgentInterface
{
	GENERATED_BODY()
public:
	ULRLocalPlayer();
	
	virtual void PostInitProperties() override;
	
	virtual void SwitchController(class APlayerController* PC) override;

	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;

protected:
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	void OnPlayerControllerChanged(APlayerController* NewController);

private:
	UPROPERTY()
	FOnTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
