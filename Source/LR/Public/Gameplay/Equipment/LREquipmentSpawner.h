// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actor/LRActor.h"
#include "LREquipmentSpawner.generated.h"

class UCapsuleComponent;

UCLASS()
class LR_API ALREquipmentSpawner : public ALRInteractableActor
{
	GENERATED_BODY()

public:
	ALREquipmentSpawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent)
	void AttemptPickUpWeapon(APawn* Pawn);
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	// UFUNCTION(BlueprintImplementableEvent, Category = "LR|WeaponPickup")
	//TODO: bool GiveWeapon(TSubclassOf<ULRInventoryItemDefinition> WeaponItemClass, APawn* ReceivingPawn);
	
protected:
	void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|WeaponPickup")
	TObjectPtr<UCapsuleComponent> CollisionVolume;

	UPROPERTY(BlueprintReadOnly, Category = "LR|WeaponPickup")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LR|WeaponPickup")
	TObjectPtr<ULREquipmentPickupInfo> EquipmentPickupInfo;

};
