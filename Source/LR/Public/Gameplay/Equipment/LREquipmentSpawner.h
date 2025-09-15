// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actor/LRActor.h"
#include "Gameplay/Inventory/LRPickupable.h"
#include "LREquipmentSpawner.generated.h"

class ULRPickupInfo;
class UCapsuleComponent;

UCLASS(Blueprintable)
class LR_API ALREquipmentSpawner : public ALRInteractableActor, public ILRPickupable
{
	GENERATED_BODY()

public:
	ALREquipmentSpawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent)
	void AttemptPickUpItem(APawn* Pawn);
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	// UFUNCTION(BlueprintImplementableEvent, Category = "LR|Pickup")
	//TODO: bool GiveWeapon(TSubclassOf<ULRInventoryItemDefinition> WeaponItemClass, APawn* ReceivingPawn);
	
protected:
	void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Pickup")
	TObjectPtr<UCapsuleComponent> CollisionVolume;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Pickup")
	TObjectPtr<UStaticMeshComponent> ItemMesh;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LR|Pickup")
	TObjectPtr<ULRInventoryItemInfo> EquipmentItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|EquipmentSpawner")
	int32 ItemStackCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|EquipmentSpawner")
	bool bAutoPickupOnOverlap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|EquipmentSpawner")
	bool bDestroyAfterPickup = true;
};
