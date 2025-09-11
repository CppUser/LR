// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Interaction/LRInteractableTarget.h"

#include "LRActor.generated.h"

UCLASS()
class LR_API ALRActor : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ALRActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Actor)
	FGameplayTagContainer StaticGameplayTags;
};

UCLASS(Abstract, Blueprintable)
class LR_API ALRInteractableActor : public ALRActor, public IInteractableTarget
{
	GENERATED_BODY()

public:
	ALRInteractableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;

	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, 
		FGameplayEventData& InOutEventData) override;

	// Check if actor can provide specific interaction type
	UFUNCTION(BlueprintPure, Category = "LR|Interaction")
	bool CanProvideInteraction(EInteractionType Type, const AActor* Interactor) const;

	// Called when interaction starts
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionStarted(EInteractionType Type, AActor* Interactor);

	// Called when interaction completes
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionCompleted(EInteractionType Type, AActor* Interactor);

	// Called when interaction is interrupted
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Interaction")
	void OnInteractionInterrupted(EInteractionType Type, AActor* Interactor);

protected:
	// Configure available interactions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactions")
	TArray<FInteractionOption> AvailableInteractions;

	// Dynamic state flags
	UPROPERTY(BlueprintReadWrite, Category = "Interactions")
	bool bIsLootable = true;

	UPROPERTY(BlueprintReadWrite, Category = "Interactions")
	bool bIsUsable = true;

	UPROPERTY(BlueprintReadWrite, Category = "Interactions")
	bool bCanBePickpocketed = false;

	UPROPERTY(BlueprintReadWrite, Category = "Interactions")
	bool bCanBeStealthKilled = false;

	// Additional properties for specific interaction types
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	// TArray<TSubclassOf<class AItem>> LootItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	int32 MinLootCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	int32 MaxLootCount = 3;
	
};