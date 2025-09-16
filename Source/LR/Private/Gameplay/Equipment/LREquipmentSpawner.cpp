// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Equipment/LREquipmentSpawner.h"

#include "Components/CapsuleComponent.h"
#include "Gameplay/Equipment/LRPickupInfo.h"


ALREquipmentSpawner::ALREquipmentSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;
	CollisionVolume->InitCapsuleSize(50.0f, 50.0f);
	CollisionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionVolume->SetCollisionProfileName(TEXT("Trigger"));
	CollisionVolume->SetGenerateOverlapEvents(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionProfileName(TEXT("NoCollision"));

	InteractionOption.InteractionType = EInteractionType::Collect;
	InteractionOption.InteractionMethod = EInteractionMethod::Proximity; 
	InteractionOption.Text = FText::FromString(TEXT("Pick Up"));
	InteractionOption.Priority = 100;
	InteractionOption.bCanBeInterrupted = true;
}

void ALREquipmentSpawner::AttemptPickUpItem_Implementation(APawn* Pawn)
{
}

void ALREquipmentSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
}

FInventoryPickup ALREquipmentSpawner::GetPickupInventory() const
{
	return FInventoryPickup{};
}

void ALREquipmentSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void ALREquipmentSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ALREquipmentSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALREquipmentSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
