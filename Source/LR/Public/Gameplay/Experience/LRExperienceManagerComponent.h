// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameStateComponent.h"
#include "LRExperienceManagerComponent.generated.h"

class ULRExperience;
namespace UE::GameFeatures { struct FResult; }

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExperienceLoaded, const ULRExperience* /*Experience*/);

enum class ExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingChaosTestingDelay,
	Loaded,
	Deactivating
};


UCLASS()
class LR_API ULRExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:
	ULRExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnExperienceLoaded::FDelegate&& Delegate);

	const ULRExperience* GetCurrentExperienceChecked() const;
	bool IsExperienceLoaded() const;
private:
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
private:
	UPROPERTY()
	TObjectPtr<const ULRExperience> CurrentExperience;

	ExperienceLoadState LoadState = ExperienceLoadState::Unloaded;

	FOnExperienceLoaded OnExperienceLoaded_HighPriority;
	FOnExperienceLoaded OnExperienceLoaded;
	FOnExperienceLoaded OnExperienceLoaded_LowPriority;
};
