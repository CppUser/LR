// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Weapon/Melee/LRGA_MeleeWeapon.h"

#include "Gameplay/Systems/Weapon/LRWeaponInstance.h"
#include "Gameplay/Systems/Weapon/Melee/LRMeleeWeaponInstance.h"
#include "Gameplay/Systems/Weapon/Tasks/LR_WaitComboWindow.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

ULRGA_MeleeWeapon::ULRGA_MeleeWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	,CurrentMontageTask(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;

	CurrentCombo = nullptr;
	CurrentAttackIndex = 0;
	bComboWindowOpen = false;
	bInputBuffered = false;
}

void ULRGA_MeleeWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("COMBO: MeleeWeapon ability activated"));
	
	if (!GetWeaponInstance())
	{
		UE_LOG(LogTemp, Error, TEXT("COMBO: No weapon instance found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!GetWeaponInstance()->GetComboData())
	{
		UE_LOG(LogTemp, Error, TEXT("COMBO: No combo data found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const FWeaponComboState& CurrentState = GetWeaponInstance()->GetComboState();
    
	if (IsActive() && CurrentState.bComboWindowOpen && !CurrentState.ActiveComboID.IsEmpty())
	{
		// We're in a combo window, buffer the input
		UE_LOG(LogTemp, Warning, TEXT("COMBO: Input during active combo window"));
		bInputBuffered = true;
		return; // Don't process yet, wait for montage to complete
	}

	ProcessComboLogic();
}

bool ULRGA_MeleeWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
    
	//TODO: Check if we're already in an attack that can't be interrupted
	// This would check animation states, etc.
    
	return true;
}

void ULRGA_MeleeWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboTimeoutHandle);
	}

	if (CurrentCombo)
	{
		K2_OnComboEnd(*CurrentCombo, !bWasCancelled && CurrentAttackIndex >= CurrentCombo->AttackSequence.Num() - 1);
	}

	if (GetWeaponInstance())
	{
		GetWeaponInstance()->ResetComboState();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULRGA_MeleeWeapon::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	UE_LOG(LogTemp, Warning, TEXT("COMBO: InputPressed called, bComboWindowOpen=%s"), 
		bComboWindowOpen ? TEXT("true") : TEXT("false"));
    
	if (bComboWindowOpen)
	{
		bInputBuffered = true;
		UE_LOG(LogTemp, Warning, TEXT("COMBO: Input buffered during window"));
	}
}

bool ULRGA_MeleeWeapon::ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayEventData* Payload) const
{
	return true;
}

void ULRGA_MeleeWeapon::PerformAttack()
{
	if (!CurrentCombo || CurrentAttackIndex >= CurrentCombo->AttackSequence.Num())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
    
	const FComboAttack& Attack = CurrentCombo->AttackSequence[CurrentAttackIndex];

	PlayAttackMontage(Attack);

	StartComboTimeout();
}

void ULRGA_MeleeWeapon::ProcessComboLogic()
{
	if (!GetWeaponInstance())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	const FWeaponComboState& ComboState = GetWeaponInstance()->GetComboState();
	float CurrentTime = GetWorld()->GetTimeSeconds();
	
	bool bContinuingCombo = false;

	if (!ComboState.ActiveComboID.IsEmpty() && ComboState.bComboWindowOpen)
	{
		// Try to continue current combo
		bContinuingCombo = TryContinueCombo();
	}
    
	if (!bContinuingCombo)
	{
		// Try to start a new combo
		if (!TryStartNewCombo())
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}
	}
    
	PerformAttack();
}

void ULRGA_MeleeWeapon::CheckComboWindow()
{
	// Called by input to check if we can buffer the next attack
	if (bComboWindowOpen)
	{
		bInputBuffered = true;
	}
}

void ULRGA_MeleeWeapon::ResetCombo()
{
	if (GetWeaponInstance())
	{
		GetWeaponInstance()->ResetComboState();
	}
    
	CurrentCombo = nullptr;
	CurrentAttackIndex = 0;
	bComboWindowOpen = false;
	bInputBuffered = false;
}

void ULRGA_MeleeWeapon::OnNotifyComboWindowOpen()
{
	bComboWindowOpen = true;
	if (GetWeaponInstance())
	{
		FWeaponComboState State = GetWeaponInstance()->GetComboState();
		State.bComboWindowOpen = true;
		// Note: This would need a non-const setter or friend access
	}

	UE_LOG(LogTemp, Warning, TEXT("COMBO: Window opened for next input"));
}

void ULRGA_MeleeWeapon::OnNotifyComboWindowClose()
{
	bComboWindowOpen = false;
    
	if (GetWeaponInstance())
	{
		FWeaponComboState& State = const_cast<FWeaponComboState&>(GetWeaponInstance()->GetComboState());
		State.bComboWindowOpen = false;
	}
    
	UE_LOG(LogTemp, Warning, TEXT("COMBO: Window closed"));
    
	// Check if we should continue combo
	if (bInputBuffered)
	{
		UE_LOG(LogTemp, Warning, TEXT("COMBO: Processing buffered input"));
		ProcessBufferedInput();
	}
}

void ULRGA_MeleeWeapon::OnMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("COMBO: Montage completed, bInputBuffered=%s, CurrentAttackIndex=%d"), 
		bInputBuffered ? TEXT("true") : TEXT("false"), CurrentAttackIndex);
    
	if (bInputBuffered && CurrentAttackIndex < CurrentCombo->AttackSequence.Num() - 1)
	{
		// Continue combo
		CurrentAttackIndex++;
		bInputBuffered = false;
		bComboWindowOpen = false;
        
		GetWeaponInstance()->SetComboState(CurrentCombo->ComboID, CurrentAttackIndex);
        
		UE_LOG(LogTemp, Warning, TEXT("COMBO: Continuing to attack %d"), CurrentAttackIndex);
		PerformAttack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("COMBO: Ending combo"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void ULRGA_MeleeWeapon::OnMontageBlendOut()
{
	OnMontageCompleted();
}

void ULRGA_MeleeWeapon::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void ULRGA_MeleeWeapon::StartComboTimeout()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimeoutHandle,
			this,
			&ULRGA_MeleeWeapon::OnComboTimeout,
			GetWeaponInstance() ? GetWeaponInstance()->GetComboData()->ComboTimeoutDuration : 1.5f,
			false
		);
	}
}

void ULRGA_MeleeWeapon::OnComboTimeout()
{
	ResetCombo();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool ULRGA_MeleeWeapon::TryStartNewCombo()
{
	if (!GetWeaponInstance() || !GetWeaponInstance()->GetComboData())
	{
		return false;
	}

	const FWeaponCombo* BestCombo = FindBestMatchingCombo(InputType, 0);
    
	if (!BestCombo)
	{
		return false;
	}
    
	CurrentCombo = BestCombo;
	CurrentAttackIndex = 0;
	GetWeaponInstance()->SetComboState(CurrentCombo->ComboID, 0);
    
	K2_OnComboStart(*CurrentCombo, 0);
    
	return true;
}

bool ULRGA_MeleeWeapon::TryContinueCombo()
{
	if (!GetWeaponInstance())
	{
		return false;
	}
    
	const FWeaponCombo* ActiveCombo = GetWeaponInstance()->GetActiveCombo();
	if (!ActiveCombo)
	{
		return false;
	}
    
	int32 NextIndex = GetWeaponInstance()->GetComboState().CurrentComboIndex + 1;
    
	// Check if we can continue this combo
	if (NextIndex >= ActiveCombo->AttackSequence.Num())
	{
		// Combo complete, check for extended combos
		const FWeaponCombo* ExtendedCombo = FindBestMatchingCombo(InputType, NextIndex);
		if (ExtendedCombo && ExtendedCombo != ActiveCombo)
		{
			// Switch to extended combo
			CurrentCombo = ExtendedCombo;
			CurrentAttackIndex = NextIndex;
			GetWeaponInstance()->SetComboState(ExtendedCombo->ComboID, NextIndex);
			return true;
		}
		return false;
	}
    
	// Check if input matches
	const FComboAttack& NextAttack = ActiveCombo->AttackSequence[NextIndex];
	if (NextAttack.RequiredInput != InputType && NextAttack.RequiredInput != EComboInputType::AnyAttack)
	{
		return false;
	}
    
	CurrentCombo = ActiveCombo;
	CurrentAttackIndex = NextIndex;
	GetWeaponInstance()->SetComboState(ActiveCombo->ComboID, NextIndex);
    
	return true;
}

const FWeaponCombo* ULRGA_MeleeWeapon::FindBestMatchingCombo(EComboInputType Input, int32 ComboIndex)
{
	if (!GetWeaponInstance() || !GetWeaponInstance()->GetComboData())
	{
		return nullptr;
	}
    
	ULRWeaponComboInfo* ComboData = GetWeaponInstance()->GetComboData();
	const FWeaponCombo* BestCombo = nullptr;
	int32 BestPriority = -1;
    
	// Check basic combo
	if (ComboIndex < ComboData->BasicCombo.AttackSequence.Num())
	{
		const FComboAttack& Attack = ComboData->BasicCombo.AttackSequence[ComboIndex];
		if ((Attack.RequiredInput == Input || Attack.RequiredInput == EComboInputType::AnyAttack) &&
			GetWeaponInstance()->CanPerformCombo(ComboData->BasicCombo))
		{
			BestCombo = &ComboData->BasicCombo;
			BestPriority = 0;
		}
	}
    
	// Check advanced combos
	for (const FWeaponCombo& Combo : ComboData->AdvancedCombos)
	{
		if (ComboIndex < Combo.AttackSequence.Num())
		{
			const FComboAttack& Attack = Combo.AttackSequence[ComboIndex];
			if ((Attack.RequiredInput == Input || Attack.RequiredInput == EComboInputType::AnyAttack) &&
				GetWeaponInstance()->CanPerformCombo(Combo))
			{
				// Prioritize longer combos and finishers
				int32 Priority = Combo.AttackSequence.Num() + (Combo.bIsFinisher ? 10 : 0);
                
				if (Priority > BestPriority)
				{
					BestCombo = &Combo;
					BestPriority = Priority;
				}
			}
		}
	}
    
	return BestCombo;
}

void ULRGA_MeleeWeapon::PlayAttackMontage(const FComboAttack& Attack)
{
	if (!Attack.AttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("COMBO: No montage for attack index %d"), CurrentAttackIndex);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("COMBO: Playing montage %s for attack %d"), 
		*Attack.AttackMontage->GetName(), CurrentAttackIndex);


	if (CurrentMontageTask)
	{
		CurrentMontageTask->EndTask();
		CurrentMontageTask = nullptr;
	}

	
	// Play montage and wait
	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Attack.AttackMontage,
		1.0f
	);
    
	if (CurrentMontageTask)
	{
		CurrentMontageTask->OnCompleted.AddDynamic(this, &ULRGA_MeleeWeapon::OnMontageCompleted);
		CurrentMontageTask->OnBlendOut.AddDynamic(this, &ULRGA_MeleeWeapon::OnMontageBlendOut);
		CurrentMontageTask->OnCancelled.AddDynamic(this, &ULRGA_MeleeWeapon::OnMontageCancelled);
		CurrentMontageTask->ReadyForActivation();
        
		// Only setup combo window for non-final attacks
		if (CurrentAttackIndex < CurrentCombo->AttackSequence.Num() - 1)
		{
			if (ComboWindowTask)
			{
				ComboWindowTask->EndTask();
				ComboWindowTask = nullptr;
			}
            
			ComboWindowTask = ULR_WaitComboWindow::WaitComboWindow(
				this,
				Attack.InputWindowStart,
				Attack.InputWindowEnd,
				GetWeaponInstance()->GetComboData()->ComboTimeoutDuration
			);
            
			if (ComboWindowTask)
			{
				ComboWindowTask->OnWindowOpen.AddDynamic(this, &ULRGA_MeleeWeapon::OnNotifyComboWindowOpen);
				ComboWindowTask->OnWindowClose.AddDynamic(this, &ULRGA_MeleeWeapon::OnNotifyComboWindowClose);
				ComboWindowTask->OnTimeout.AddDynamic(this, &ULRGA_MeleeWeapon::OnComboTimeout);
				ComboWindowTask->ReadyForActivation();
			}
		}
	}
  
}

ULRWeaponInstance* ULRGA_MeleeWeapon::GetWeaponInstance() const
{
	return Cast<ULRWeaponInstance>(GetAssociatedEquipment());
}

void ULRGA_MeleeWeapon::ProcessBufferedInput()
{
	if (!bInputBuffered || !CurrentCombo)
	{
		return;
	}
    
	if (CurrentAttackIndex < CurrentCombo->AttackSequence.Num() - 1)
	{
		CurrentAttackIndex++;
		bInputBuffered = false;
		bComboWindowOpen = false;
        
		GetWeaponInstance()->SetComboState(CurrentCombo->ComboID, CurrentAttackIndex);
        
		UE_LOG(LogTemp, Warning, TEXT("COMBO: Continuing to attack %d"), CurrentAttackIndex);
		PerformAttack();
	}
}
