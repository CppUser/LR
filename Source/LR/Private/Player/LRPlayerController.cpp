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
	BroadcastOnPlayerStateChanged();
}

void ALRPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
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

void ALRPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	UE_LOG(LogTemp, Error, TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
}

FGenericTeamId ALRPlayerController::GetGenericTeamId() const
{
	if (const ILRTeamAgentInterface* PSWithTeamInterface = Cast<ILRTeamAgentInterface>(PlayerState))
	{
		return PSWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnTeamIndexChangedDelegate* ALRPlayerController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ALRPlayerController::OnPlayerStateChanged()
{
}

void ALRPlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void ALRPlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	// Unbind from the old player state, if any
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (LastSeenPlayerState != nullptr)
	{
		if (ILRTeamAgentInterface* PlayerStateTeamInterface = Cast<ILRTeamAgentInterface>(LastSeenPlayerState))
		{
			OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
		}
	}

	// Bind to the new player state, if any
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (PlayerState != nullptr)
	{
		if (ILRTeamAgentInterface* PlayerStateTeamInterface = Cast<ILRTeamAgentInterface>(PlayerState))
		{
			NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
		}
	}

	// Broadcast the team change (if it really has)
	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	LastSeenPlayerState = PlayerState;
}
