// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Player/LRPlayerState.h"

#include "AbilitySystem/LRAbilitySet.h"
#include "AbilitySystem/Attributes/LRCombatAttribSet.h"
#include "AbilitySystem/Attributes/LRHealthAttribSet.h"
#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Character/LRPawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Gameplay/Experience/LRExperienceManagerComponent.h"
#include "Gameplay/GameMode/LRGameMode.h"
#include "Player/LRPlayerController.h"

const FName ALRPlayerState::NAME_AbilityReady("ASAbilityReady");

ALRPlayerState::ALRPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ASC = ObjectInitializer.CreateDefaultSubobject<ULRAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(false);
	//ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = CreateDefaultSubobject<ULRHealthAttribSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<ULRCombatAttribSet>(TEXT("CombatSet"));

}

ALRPlayerController* ALRPlayerState::GetLRPlayerController() const
{
	return Cast<ALRPlayerController>(GetOwner());
}

UAbilitySystemComponent* ALRPlayerState::GetAbilitySystemComponent() const
{
	return GetLRAbilitySystemComponent();
}

void ALRPlayerState::SetPawnData(const ULRPawnData* InPawnData)
{
	check(InPawnData);


	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	for (const ULRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(ASC, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_AbilityReady);
}

void ALRPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALRPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(ASC);
	ASC->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		ULRExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULRExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void ALRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	//@TODO: Copy stats
}

void ALRPlayerState::OnExperienceLoaded(const ULRExperience* CurrentExperience)
{
	if (ALRGameMode* GM = GetWorld()->GetAuthGameMode<ALRGameMode>())
	{
		if (const ULRPawnData* NewPawnData = GM->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ALyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}
