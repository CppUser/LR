// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Systems/Equipment/LRGA_FromEquipment.h"
#include "Gameplay/Systems/Weapon/LRWeaponComboInfo.h"
#include "LRGA_MeleeWeapon.generated.h"


class UAbilityTask_PlayMontageAndWait;
class ULRWeaponInstance;
class ULRMeleeWeaponInstance;

UCLASS()
class LR_API ULRGA_MeleeWeapon : public ULRGA_FromEquipment
{
	GENERATED_BODY()
public:
	ULRGA_MeleeWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
			const FGameplayAbilityActorInfo* ActorInfo,
			const FGameplayAbilityActivationInfo ActivationInfo,
			const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual bool ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* Payload) const override;
protected:
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ProcessComboLogic();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void CheckComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnNotifyComboWindowOpen();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnNotifyComboWindowClose();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnMontageCompleted();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnMontageBlendOut();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnMontageCancelled();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combo")
	void K2_OnComboStart(const FWeaponCombo& Combo, int32 AttackIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combo")
	void K2_OnComboEnd(const FWeaponCombo& Combo, bool bCompleted);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combo")
	void K2_OnAttackHit(const FComboAttack& Attack);
private:
	void StartComboTimeout();
	UFUNCTION()
	void OnComboTimeout();
	bool TryStartNewCombo();
	bool TryContinueCombo();
	
	const FWeaponCombo* FindBestMatchingCombo(EComboInputType Input, int32 ComboIndex);
	void PlayAttackMontage(const FComboAttack& Attack);

	ULRWeaponInstance* GetWeaponInstance() const;

	void ProcessBufferedInput();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	EComboInputType InputType = EComboInputType::LightAttack;

	const FWeaponCombo* CurrentCombo;
private:
	int32 CurrentAttackIndex;
	bool bComboWindowOpen;
	bool bInputBuffered;
	FTimerHandle ComboTimeoutHandle;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentMontageTask;
	UPROPERTY()
	class ULR_WaitComboWindow* ComboWindowTask = nullptr;
};
