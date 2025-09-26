// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "ModularPlayerState.h"
#include "Teams/ILRTeamAgentInterface.h"
#include "Utils/LRGameplayTagStack.h"
#include "LRPlayerState.generated.h"


class ULRExperience;
class ULRPawnData;
class ULRAbilitySystemComponent;
class ALRPlayerController;

UCLASS(Config = Game)
class LR_API ALRPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public ILRTeamAgentInterface
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

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;

	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

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

	UPROPERTY()
	TObjectPtr<const class ULRHealthAttribSet> HealthSet;
	UPROPERTY()
	TObjectPtr<const class ULRCombatAttribSet> CombatSet;

	UPROPERTY()
	FOnTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FGameplayTagStackContainer StatTags;
};
