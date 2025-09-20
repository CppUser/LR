// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Systems/Interaction/LRInteractableTarget.h"

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
class LR_API ALRInteractableActor : public ALRActor, public ILRInteractableTarget
{
	GENERATED_BODY()

public:
	ALRInteractableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GatherInteractionOptions(const FLRInteractionQuery& InteractQuery, FLRInteractionOptionBuilder& OptionBuilder) override;

	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, 
		FGameplayEventData& InOutEventData) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	FLRInteractionOption InteractionOption;
	
};