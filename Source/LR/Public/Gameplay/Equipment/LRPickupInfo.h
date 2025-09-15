// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LRPickupInfo.generated.h"

class UNiagaraSystem;


UCLASS(Blueprintable, BlueprintType, Const)
class LR_API ULRPickupInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup|Mesh")
	TObjectPtr<UStaticMesh> DisplayMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup")
	TObjectPtr<USoundBase> PickedUpSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup")
	TObjectPtr<UNiagaraSystem> PickedUpEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup|Equipment")
	TSubclassOf<ULRInventoryItemInfo> InventoryItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup|Mesh")
	FVector MeshOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup|Mesh")
	FVector MeshScale = FVector(1.0f, 1.0f, 1.0f);
};
