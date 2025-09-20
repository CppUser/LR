// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/LRAbilitySet.h"
#include "Components/PawnComponent.h"
#include "LREquipmentManagerComponent.generated.h"


class ULREquipmentInstance;
class ULREquipmentInfo;

USTRUCT(BlueprintType)
struct FEquipmentEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<ULREquipmentInfo> EquipmentInfo;
	
	UPROPERTY()
	TObjectPtr<ULREquipmentInstance> Instance;

	UPROPERTY()
	FAbilitySet_GrantedHandles GrantedAbilities;

	friend struct FEquipmentList;
	friend class ULREquipmentManagerComponent;
};

USTRUCT(BlueprintType)
struct FEquipmentList
{
	GENERATED_BODY()

	FEquipmentList() : Owner(nullptr) {};
	FEquipmentList(UActorComponent* InComponent) : Owner(InComponent) {};

	ULREquipmentInstance* AddEntry(TSubclassOf<ULREquipmentInfo> InEquipmentInfo);
	void RemoveEntry(ULREquipmentInstance* InInstance);

private:
	ULRAbilitySystemComponent* GetASC() const;

private:
	UPROPERTY()
	TArray<FEquipmentEntry> Equipment;
	UPROPERTY()
	TObjectPtr<UActorComponent> Owner;

	friend class ULREquipmentManagerComponent;
};

UCLASS(BlueprintType, Const)
class LR_API ULREquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	ULREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	ULREquipmentInstance* EquipItem(TSubclassOf<ULREquipmentInfo> EquipmentDefinition);

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
	UPROPERTY()
	FEquipmentList EquipmentList;
};
