// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "LREquipmentInfo.generated.h"

class UGameplayEffect;
class ULRAbilitySet;
class ULREquipmentInstance;

UENUM(BlueprintType)
enum class EEquipmentPersonality : uint8
{
	None,
	Aggressive,     // Pushes risky play, boosts crit but lowers defense
	Protective,     // Empowers defensive stances, healing, control
	Chaotic,        // Randomly changes properties each fight
	Parasitic,      // Feeds off player but grants powerful bonuses
	Symbiotic,      // Grows stronger with long-term bonding
	Sentient        // Has complex AI-driven personality and goals
};

UENUM(BlueprintType)
enum class EEquipmentEvolutionTrigger : uint8
{
	PlayerBehavior,     // Based on moral choices (kill innocents vs save)
	CombatStyle,        // How weapon is used (aggressive vs defensive)
	Environment,        // Exposure to different biomes/elements
	TimeEquipped,       // Length of time equipped
	KillCount,          // Number of enemies killed
	DamageDealt,        // Total damage output
	DamageReceived,     // Damage absorbed (for armor/shields)
	AbilityUse,         // Specific abilities used while equipped
	ElementalExposure,  // Exposure to fire, ice, poison, etc.
	BiomeExposure      // Time spent in different environments
};

USTRUCT(BlueprintType)
struct FEquipmentEvolutionStage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	FText StageDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	TMap<EEquipmentEvolutionTrigger, float> RequiredTriggerValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	TArray<TObjectPtr<const ULRAbilitySet>> GrantedAbilitySets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	FGameplayTagContainer GrantedTags;

	// Visual changes for this evolution stage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<USkeletalMesh> EvolvedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TArray<TObjectPtr<UMaterialInterface>> EvolvedMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> EvolutionSound;
};

USTRUCT(BlueprintType)
struct FEquipmentPersonalityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Personality")
	EEquipmentPersonality PersonalityType = EEquipmentPersonality::None;

	// Dialogue lines for different situations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Personality")
	TArray<FText> OnEquipDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Personality")
	TArray<FText> OnCombatDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Personality")
	TArray<FText> OnKillDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Personality")
	TArray<FText> OnIdleDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Personality")
	TArray<FText> OnUnequipDialogue;

	// Personality goals and reactions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goals")
	FGameplayTagContainer PreferredTargets; // e.g., "Enemy.Type.Mage" for mage-killing weapons

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goals")
	FGameplayTagContainer HatedActions; // Actions that anger the item

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goals")
	FGameplayTagContainer LovedActions; // Actions that please the item

	// Mood affects performance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	float HappinessBonusMultiplier = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	float AngerPenaltyMultiplier = 0.8f;

	// Rebellion - item may refuse to work if too angry
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rebellion")
	float RebellionThreshold = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rebellion")
	float RebellionChance = 0.1f; // 10% chance to refuse activation
};

USTRUCT(BlueprintType)
struct FEquipmentResonanceRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resonance")
	FGameplayTagContainer RequiredItemTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resonance")
	FText ResonanceName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resonance")
	FText ResonanceDescription;

	// Positive resonance effects
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> BonusEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TArray<TObjectPtr<const ULRAbilitySet>> BonusAbilities;

	// Negative dissonance effects
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> PenaltyEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	float EffectivenessMultiplier = 1.0f;
};

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

	UPROPERTY(EditAnywhere, Category=Equipment)
	int32 MinEvolutionStage = 0;

	UPROPERTY(EditAnywhere, Category=Equipment)
	int32 MaxEvolutionStage = 999;
};


UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class LR_API ULREquipmentInfo : public UObject
{
	GENERATED_BODY()
public:
	ULREquipmentInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category="LR|Equipment")
	TSubclassOf<ULREquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category="LR|Equipment")
	TArray<TObjectPtr<const ULRAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category="LR|Equipment")
	TArray<FEquipmentActorToSpawn> ActorsToSpawn;
		
	UPROPERTY(EditDefaultsOnly, Category="LR|Evolution", meta=(TitleProperty="StageName"))
    TArray<FEquipmentEvolutionStage> EvolutionStages;

    UPROPERTY(EditDefaultsOnly, Category="LR|Evolution")
    bool bCanEvolve = false;

    UPROPERTY(EditDefaultsOnly, Category="LR|Evolution")
    float EvolutionRateMultiplier = 1.0f;

    // Personality System
    UPROPERTY(EditDefaultsOnly, Category="LR|Personality")
    FEquipmentPersonalityData PersonalityData;

    // Resonance System
    UPROPERTY(EditDefaultsOnly, Category="LR|Resonance", meta=(TitleProperty="ResonanceName"))
    TArray<FEquipmentResonanceRule> ResonanceRules;

    // Mystery Item System
    UPROPERTY(EditDefaultsOnly, Category="LR|Mystery")
    bool bIsMysteryItem = false;

    UPROPERTY(EditDefaultsOnly, Category="LR|Mystery")
    FText HiddenName;

    UPROPERTY(EditDefaultsOnly, Category="LR|Mystery")
    FText HiddenDescription;

    UPROPERTY(EditDefaultsOnly, Category="LR|Mystery")
    TArray<FGameplayTag> UnlockMethods; // Methods to reveal item properties

    // Environmental Synergy
    UPROPERTY(EditDefaultsOnly, Category="LR|Environment")
    TMap<FGameplayTag, float> EnvironmentBonuses; // Biome tag -> effectiveness multiplier

    UPROPERTY(EditDefaultsOnly, Category="LR|Environment")
    TMap<FGameplayTag, float> EnvironmentPenalties;

    // Living Item Properties
    UPROPERTY(EditDefaultsOnly, Category="LR|Living")
    bool bIsLivingItem = false;

    UPROPERTY(EditDefaultsOnly, Category="LR|Living")
    float MaxLifespan = -1.0f; // -1 = infinite, otherwise dies after time

    UPROPERTY(EditDefaultsOnly, Category="LR|Living")
    float GrowthRate = 1.0f; // How fast it grows stronger

    UPROPERTY(EditDefaultsOnly, Category="LR|Living")
    float HungerRate = 0.0f; // If > 0, needs to "feed" on kills/damage

    // Parasitic Properties
    UPROPERTY(EditDefaultsOnly, Category="LR|Parasitic")
    bool bIsParasitic = false;

    UPROPERTY(EditDefaultsOnly, Category="LR|Parasitic")
    float HealthDrainRate = 0.0f; // HP drained per second

    UPROPERTY(EditDefaultsOnly, Category="LR|Parasitic")
    float XPDrainRate = 0.0f; // XP drained per kill

    UPROPERTY(EditDefaultsOnly, Category="LR|Parasitic")
    float PowerBonus = 1.5f;
};
