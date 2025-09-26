// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularAIController.h"
#include "AbilitySystem/LRAbilitySet.h"
#include "LRAIController.generated.h"

UCLASS()
class LR_API ALRAIController : public AModularAIController , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ALRAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "LR|PlayerState")
	ULRAbilitySystemComponent* GetLRAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
private:
	UPROPERTY(EditDefaultsOnly, Category="LR|Ability")
	TObjectPtr<ULRAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="LR|Ability")
	TObjectPtr<ULRAbilitySet> AbilitySetOnSpawn;

	UPROPERTY(VisibleInstanceOnly, Category="LR|Ability")
	FAbilitySet_GrantedHandles GrantedHandlesOnSpawn;
};
