// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Weapon/Tasks/LR_WaitComboWindow.h"

ULR_WaitComboWindow* ULR_WaitComboWindow::WaitComboWindow(UGameplayAbility* OwningAbility, float WindowStartTime,
	float WindowEndTime, float Timeout)
{
	ULR_WaitComboWindow* Task = NewAbilityTask<ULR_WaitComboWindow>(OwningAbility);
	Task->WindowStart = WindowStartTime;
	Task->WindowEnd = WindowEndTime;
	Task->TimeoutDuration = Timeout;
	
	UE_LOG(LogTemp, Warning, TEXT("COMBO WINDOW: Task created - Start:%f, End:%f, Timeout:%f"), 
		WindowStartTime, WindowEndTime, Timeout);
	
	return Task;
}

void ULR_WaitComboWindow::Activate()
{
	UE_LOG(LogTemp, Warning, TEXT("COMBO WINDOW: Activating task"));
    
	if (UWorld* World = GetWorld())
	{
		// Start window timer
		World->GetTimerManager().SetTimer(
			StartTimer,
			this,
			&ULR_WaitComboWindow::OnWindowStarted,
			WindowStart,
			false
		);
        
		// End window timer  
		World->GetTimerManager().SetTimer(
			EndTimer,
			this,
			&ULR_WaitComboWindow::OnWindowEnded,
			WindowEnd,
			false
		);
        
		// Timeout timer
		World->GetTimerManager().SetTimer(
			TimeoutTimer,
			this,
			&ULR_WaitComboWindow::OnTimedOut,
			TimeoutDuration,
			false
		);
        
		UE_LOG(LogTemp, Warning, TEXT("COMBO WINDOW: Timers set"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("COMBO WINDOW: No world!"));
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
	UE_LOG(LogTemp, Warning, TEXT("COMBO WINDOW: Window STARTED"));
	OnWindowOpen.Broadcast();
}

void ULR_WaitComboWindow::OnWindowEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("COMBO WINDOW: Window ENDED"));
	OnWindowClose.Broadcast();
}

void ULR_WaitComboWindow::OnTimedOut()
{
	UE_LOG(LogTemp, Warning, TEXT("COMBO WINDOW: TIMEOUT"));
	OnTimeout.Broadcast();
	EndTask();
}
