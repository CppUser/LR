// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LREquipmentInfo.generated.h"

class ULRAbilitySet;
class ULREquipmentInstance;

USTRUCT()
struct FEquipmentActorToSpawn
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,Category = "Equipment")
	TSubclassOf<AActor> ActorClass;
	
	UPROPERTY(EditDefaultsOnly,Category = "Equipment")
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly,Category = "Equipment")
	FTransform AttachTransform;
};


UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class LR_API ULREquipmentInfo : public UObject
{
	GENERATED_BODY()
public:
	ULREquipmentInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<ULREquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const ULRAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FEquipmentActorToSpawn> ActorsToSpawn;
		
	
};
