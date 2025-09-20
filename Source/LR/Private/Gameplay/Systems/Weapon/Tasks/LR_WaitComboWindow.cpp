// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Weapon/Tasks/LR_WaitComboWindow.h"

ULR_WaitComboWindow* ULR_WaitComboWindow::WaitComboWindow(UGameplayAbility* OwningAbility, float WindowStartTime,
	float WindowEndTime, float Timeout)
{
	ULR_WaitComboWindow* Task = NewAbilityTask<ULR_WaitComboWindow>(OwningAbility);
	Task->WindowStart = WindowStartTime;
	Task->WindowEnd = WindowEndTime;
	Task->TimeoutDuration = Timeout;
	return Task;
}

void ULR_WaitComboWindow::Activate()
{
	if (GetWorld())
	{
		// Start window timer
		GetWorld()->GetTimerManager().SetTimer(
			StartTimer,
			this,
			&ULR_WaitComboWindow::OnWindowStarted,
			WindowStart,
			false
		);
        
		// End window timer
		GetWorld()->GetTimerManager().SetTimer(
			EndTimer,
			this,
			&ULR_WaitComboWindow::OnWindowEnded,
			WindowEnd,
			false
		);
        
		// Timeout timer
		GetWorld()->GetTimerManager().SetTimer(
			TimeoutTimer,
			this,
			&ULR_WaitComboWindow::OnTimedOut,
			TimeoutDuration,
			false
		);
	}
}

void ULR_WaitComboWindow::OnDestroy(bool AbilityEnded)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(StartTimer);
		GetWorld()->GetTimerManager().ClearTimer(EndTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);
	}
	
	Super::OnDestroy(AbilityEnded);
}

void ULR_WaitComboWindow::OnWindowStarted()
{
	OnWindowOpen.Broadcast();
}

void ULR_WaitComboWindow::OnWindowEnded()
{
	OnWindowClose.Broadcast();
}

void ULR_WaitComboWindow::OnTimedOut()
{
	OnTimeout.Broadcast();
	EndTask();
}
