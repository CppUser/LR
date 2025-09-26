// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "Teams/ILRTeamAgentInterface.h"
#include "LRCharacter.generated.h"

class ULRInventoryManagerComponent;
class ULREquipmentManagerComponent;
class ULRAbilitySystemComponent;
class ALRPlayerState;
class ALRPlayerController;
class ULRPawnExtComponent;


//TODO: Setup Character base class so AI can derive from it , currently ai derive from LRCharacter
//Not all components need to ai
UCLASS(Config = Game)
class LR_API ALRCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public ILRTeamAgentInterface
{
	GENERATED_BODY()

public:
	ALRCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "LR|Character")
	ALRPlayerController* GetLRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Character")
	ALRPlayerState* GetLRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Character")
	ULRAbilitySystemComponent* GetLRAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;

	virtual void NotifyControllerChanged() override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
protected:
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		return FGenericTeamId::NoTeam;
	}
private:
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULRPawnExtComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULREquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULRInventoryManagerComponent> InventoryManagerComponent; //TODO: Place in controller ?

	UPROPERTY()
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FOnTeamIndexChangedDelegate OnTeamChangedDelegate;
};
