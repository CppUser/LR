// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "LRCharacter.generated.h"

class ULRInventoryManagerComponent;
class ULREquipmentManagerComponent;
class ULRAbilitySystemComponent;
class ALRPlayerState;
class ALRPlayerController;
class ULRPawnExtComponent;

UCLASS(Config = Game)
class LR_API ALRCharacter : public AModularCharacter, public IAbilitySystemInterface
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
protected:
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULRPawnExtComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULREquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULRInventoryManagerComponent> InventoryManagerComponent; //TODO: Place in controller ?
};
