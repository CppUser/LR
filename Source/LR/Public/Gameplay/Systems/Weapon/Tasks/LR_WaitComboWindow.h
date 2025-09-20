// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "LR_WaitComboWindow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FComboWindowEvent);


UCLASS()
class LR_API ULR_WaitComboWindow : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static ULR_WaitComboWindow* WaitComboWindow(
		UGameplayAbility* OwningAbility,
		float WindowStartTime,
		float WindowEndTime,
		float Timeout
	);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

public:
	UPROPERTY(BlueprintAssignable)
	FComboWindowEvent OnWindowOpen;

	UPROPERTY(BlueprintAssignable)
	FComboWindowEvent OnWindowClose;

	UPROPERTY(BlueprintAssignable)
	FComboWindowEvent OnTimeout;
protected:
	UFUNCTION()
	void OnWindowStarted();

	UFUNCTION()
	void OnWindowEnded();

	UFUNCTION()
	void OnTimedOut();
protected:
	float WindowStart;
	float WindowEnd;
	float TimeoutDuration;

	FTimerHandle StartTimer;
	FTimerHandle EndTimer;
	FTimerHandle TimeoutTimer;

	
};
