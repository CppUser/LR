// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Player/LRPlayerController.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Player/LRPlayerState.h"
#include "UI/LRHUD.h"

ALRPlayerController::ALRPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

ALRPlayerState* ALRPlayerController::GetLRPlayerState() const
{
	return CastChecked<ALRPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

ULRAbilitySystemComponent* ALRPlayerController::GetLRAbilitySystemComponent() const
{
	const ALRPlayerState* PS = GetLRPlayerState();
	return (PS ? PS->GetLRAbilitySystemComponent() : nullptr);
}

ALRHUD* ALRPlayerController::GetLRHUD() const
{
	return CastChecked<ALRHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void ALRPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void ALRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ALRPlayerController::OnUnPossess()
{
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}
	
	Super::OnUnPossess();
}

void ALRPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

void ALRPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
}

void ALRPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ALRPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ALRPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

void ALRPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ALRPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (ULRAbilitySystemComponent* ASC = GetLRAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}
