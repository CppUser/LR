// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "LRPlayerController.generated.h"


class ALRHUD;
class ULRAbilitySystemComponent;
class ALRPlayerState;

UCLASS(Config = Game)
class LR_API ALRPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	ALRPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "LR|PlayerController")
	ALRPlayerState* GetLRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "LR|PlayerController")
	ULRAbilitySystemComponent* GetLRAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "LR|PlayerController")
	ALRHUD* GetLRHUD() const;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;

	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;

	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
