// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "LRPawnExtComponent.generated.h"


class ULRAbilitySystemComponent;
class ULRPawnData;

UCLASS()
class LR_API ULRPawnExtComponent : public UPawnComponent , public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	ULRPawnExtComponent(const FObjectInitializer& ObjInit = FObjectInitializer::Get());

	static ULRPawnExtComponent* FindPawnExtComponent(const AActor* InActor) {return (InActor ? InActor->FindComponentByClass<ULRPawnExtComponent>() : nullptr);}

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const ULRPawnData* InPawnData);

	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

	virtual FName GetFeatureName() const override {return NAME_ActorFeatureName;}
	
	void InitializeAbilitySystem(ULRAbilitySystemComponent* InASC, AActor* OwnerActor);
	void UninitializeAbilitySystem();

	void HandleControllerChanged();
	void SetupPlayerInputComponent();

	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	UFUNCTION(BlueprintPure)
	ULRAbilitySystemComponent* GetLRAbilitySystemComponent() const {return ASC;}
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	static const FName NAME_ActorFeatureName;
protected:
	UPROPERTY(EditInstanceOnly,Category = "LR|Pawn")
	TObjectPtr<const ULRPawnData> PawnData;
	
	UPROPERTY()
	TObjectPtr<ULRAbilitySystemComponent> ASC;

	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};
