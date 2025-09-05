// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularGameMode.h"
#include "LRGameMode.generated.h"

class ULRExperience;
class ULRPawnData;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameModePlayerInitialized, AGameModeBase* /*GameMode*/,
                                     AController* /*NewPlayer*/);

UCLASS()
class LR_API ALRGameMode : public AModularGameModeBase
{
	GENERATED_BODY()
public:
	ALRGameMode(const FObjectInitializer& ObjInit = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable,Category = "LR|Pawn")
	const ULRPawnData* GetPawnDataForController(const AController* InController) const;
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void InitGameState() override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;

	virtual bool ControllerCanRestart(AController* Controller);
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	FOnGameModePlayerInitialized OnGameModePlayerInitialized;
protected:
	void OnExperienceLoaded(const ULRExperience* CurrentExperience);
	bool IsExperienceLoaded() const;

	void PrepForExperienceLoading();
	void OnExperienceDataReady(FPrimaryAssetId ExperienceId,const FString& ExperienceSource);
};
