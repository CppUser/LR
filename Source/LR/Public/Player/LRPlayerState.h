// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "LRPlayerState.generated.h"


class ULRExperience;
class ULRPawnData;
class ULRAbilitySystemComponent;
class ALRPlayerController;

UCLASS(Config = Game)
class LR_API ALRPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ALRPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "LR|PlayerState")
	ALRPlayerController* GetLRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "LR|PlayerState")
	ULRAbilitySystemComponent* GetLRAbilitySystemComponent() const { return ASC; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const ULRPawnData* InPawnData);

	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;
private:
	void OnExperienceLoaded(const ULRExperience* CurrentExperience);
public:
	static const FName NAME_AbilityReady;
protected:
	UPROPERTY()
	TObjectPtr<const ULRPawnData> PawnData;
private:
	UPROPERTY(VisibleAnywhere, Category = "LR|PlayerState")
	TObjectPtr<ULRAbilitySystemComponent> ASC;

};
