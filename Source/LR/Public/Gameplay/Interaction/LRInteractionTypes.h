// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "LRInteractionTypes.generated.h"



UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None            UMETA(DisplayName = "None"),
	Collect         UMETA(DisplayName = "Collect/Loot"),
	Talk            UMETA(DisplayName = "Talk"),
	Use             UMETA(DisplayName = "Use/Activate"),
	Examine         UMETA(DisplayName = "Examine"),
	Mount           UMETA(DisplayName = "Mount/Ride"),
	Open            UMETA(DisplayName = "Open"),
	Custom          UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class EInteractionMethod : uint8
{
	Proximity       UMETA(DisplayName = "Proximity Based"),
	LineTrace       UMETA(DisplayName = "Looking At"),
	MouseOver       UMETA(DisplayName = "Mouse Over"),
	Combined        UMETA(DisplayName = "Combined")
};

USTRUCT(BlueprintType)
struct FInteractionRequirement
{
	GENERATED_BODY()

	// Required tags on the interactor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	// Tags that block this interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTags;

	// Minimum distance for interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistance = 0.0f;

	// Maximum distance for interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 500.0f;

	// Required angle (0 = must be behind, 90 = side, 180 = front)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RequiredAngle = -1.0f; // -1 means any angle

	// Is line of sight required?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequiresLineOfSight = true;

	bool CheckRequirements(const AActor* Interactor, const AActor* Target) const;
};