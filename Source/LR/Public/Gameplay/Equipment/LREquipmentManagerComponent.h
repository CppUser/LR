// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/LRAbilitySet.h"
#include "Components/PawnComponent.h"
#include "UObject/Object.h"
#include "LREquipmentManagerComponent.generated.h"

class ULREquipmentInstance;
class ULREquipmentDefinition;
struct FEquipmentList;
class ULREquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FAppliedEquipmentEntry
{
	GENERATED_BODY()
	friend FEquipmentList;
	friend ULREquipmentManagerComponent;
public:
	FString GetDebugString() const;

private:
	UPROPERTY()
	TSubclassOf<ULREquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<ULREquipmentInstance> Instance = nullptr;

	UPROPERTY()
	FAbilitySet_GrantedHandles GrantedHandles;
};

USTRUCT(BlueprintType)
struct FEquipmentList 
{
	GENERATED_BODY()
	friend ULREquipmentManagerComponent;
public:
	FEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	ULREquipmentInstance* AddEntry(TSubclassOf<ULREquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(ULREquipmentInstance* Instance);

private:
	ULRAbilitySystemComponent* GetAbilitySystemComponent() const;

private:
	UPROPERTY()
	TArray<FAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

UCLASS(BlueprintType, Const)
class LR_API ULREquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	ULREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	ULREquipmentInstance* EquipItem(TSubclassOf<ULREquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(ULREquipmentInstance* ItemInstance);
	
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ULREquipmentInstance* GetFirstInstanceOfType(TSubclassOf<ULREquipmentInstance> InstanceType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<ULREquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<ULREquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}
private:
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;
};
