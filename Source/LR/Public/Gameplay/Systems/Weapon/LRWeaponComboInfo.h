// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "LRWeaponComboInfo.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EComboInputType : uint8
{
	LightAttack,
	HeavyAttack,
	SpecialAttack,
	AnyAttack      
};

USTRUCT(BlueprintType)
struct FComboAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EComboInputType RequiredInput = EComboInputType::LightAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputWindowStart = 0.5f; // When in the animation can you input next

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputWindowEnd = 0.9f; // End of input window

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AttackTag;
};


USTRUCT(BlueprintType)
struct FWeaponCombo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ComboID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ComboName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FComboAttack> AttackSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFinisher = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> FinisherEffect;
};


USTRUCT(BlueprintType)
struct FWeaponComboState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentComboIndex = 0; // Which attack in the sequence

	UPROPERTY(BlueprintReadOnly)
	FString ActiveComboID;

	UPROPERTY(BlueprintReadOnly)
	float LastAttackTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bComboWindowOpen = false;

	void Reset()
	{
		CurrentComboIndex = 0;
		ActiveComboID = "";
		LastAttackTime = 0.0f;
		bComboWindowOpen = false;
	}
};

UCLASS(Blueprintable, Const)
class LR_API ULRWeaponComboInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	const FWeaponCombo* FindComboByID(const FString& ComboID) const;

	UFUNCTION(BlueprintPure, Category = "Combos")
	int32 GetMaxComboLength() const;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combos")
	FWeaponCombo BasicCombo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combos")
	TArray<FWeaponCombo> AdvancedCombos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combos")
	float ComboTimeoutDuration = 1.5f;

	
};