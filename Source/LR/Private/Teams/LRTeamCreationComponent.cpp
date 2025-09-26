// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/LRTeamCreationComponent.h"

#include "GenericTeamAgentInterface.h"
#include "Gameplay/Experience/LRExperienceManagerComponent.h"
#include "Gameplay/GameMode/LRGameMode.h"
#include "Player/LRPlayerState.h"
#include "Teams/LRTeamInfo.h"

ULRTeamCreationComponent::ULRTeamCreationComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	TeamInfoClass = ALRTeamInfo::StaticClass();
}

void ULRTeamCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	ULRExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULRExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

#if WITH_EDITOR
EDataValidationResult ULRTeamCreationComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	//@TODO: TEAMS: Validate that all display assets have the same properties set!

	return Result;
}
#endif

void ULRTeamCreationComponent::CreateTeams()
{
	for (const auto& KVP : TeamsToCreate)
	{
		const int32 TeamId = KVP.Key;
		CreateTeam(TeamId, KVP.Value);
	}
}

void ULRTeamCreationComponent::AssignPlayersToTeams()
{
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ALRPlayerState* LRPS = Cast<ALRPlayerState>(PS))
		{
			ChooseTeamForPlayer(LRPS);
		}
	}

	ALRGameMode* GameMode = Cast<ALRGameMode>(GameState->AuthorityGameMode);
	check(GameMode);

	GameMode->OnGameModePlayerInitialized.AddUObject(this, &ThisClass::OnPlayerInitialized);
}

void ULRTeamCreationComponent::ChooseTeamForPlayer(ALRPlayerState* PS)
{
	if (PS->IsOnlyASpectator())
	{
		PS->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
	else
	{
		const FGenericTeamId TeamID = IntegerToGenericTeamId(GetLeastPopulatedTeamID());
		PS->SetGenericTeamId(TeamID);
	}
}

void ULRTeamCreationComponent::OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer)
{
	check(NewPlayer);
	check(NewPlayer->PlayerState);
	if (ALRPlayerState* LRPS = Cast<ALRPlayerState>(NewPlayer->PlayerState))
	{
		ChooseTeamForPlayer(LRPS);
	}
}

void ULRTeamCreationComponent::CreateTeam(int32 TeamId, ULRTeamDisplayAsset* DisplayAsset)
{

	//@TODO: ensure the team doesn't already exist

	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALRTeamInfo* NewTeamPublicInfo = World->SpawnActor<ALRTeamInfo>(TeamInfoClass, SpawnInfo);
	checkf(NewTeamPublicInfo != nullptr, TEXT("Failed to create public team actor from class %s"), *GetPathNameSafe(*TeamInfoClass));
	NewTeamPublicInfo->SetTeamId(TeamId);
	NewTeamPublicInfo->SetTeamDisplayAsset(DisplayAsset);
}

int32 ULRTeamCreationComponent::GetLeastPopulatedTeamID() const
{
	const int32 NumTeams = TeamsToCreate.Num();
	if (NumTeams > 0)
	{
		TMap<int32, uint32> TeamMemberCounts;
		TeamMemberCounts.Reserve(NumTeams);

		for (const auto& KVP : TeamsToCreate)
		{
			const int32 TeamId = KVP.Key;
			TeamMemberCounts.Add(TeamId, 0);
		}

		AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (ALRPlayerState* LRPS = Cast<ALRPlayerState>(PS))
			{
				const int32 PlayerTeamID = LRPS->GetTeamId();

				if ((PlayerTeamID != INDEX_NONE) && !LRPS->IsInactive())	// do not count unassigned or disconnected players
				{
					check(TeamMemberCounts.Contains(PlayerTeamID))
					TeamMemberCounts[PlayerTeamID] += 1;
				}
			}
		}

		// sort by lowest team population, then by team ID
		int32 BestTeamId = INDEX_NONE;
		uint32 BestPlayerCount = TNumericLimits<uint32>::Max();
		for (const auto& KVP : TeamMemberCounts)
		{
			const int32 TestTeamId = KVP.Key;
			const uint32 TestTeamPlayerCount = KVP.Value;

			if (TestTeamPlayerCount < BestPlayerCount)
			{
				BestTeamId = TestTeamId;
				BestPlayerCount = TestTeamPlayerCount;
			}
			else if (TestTeamPlayerCount == BestPlayerCount)
			{
				if ((TestTeamId < BestTeamId) || (BestTeamId == INDEX_NONE))
				{
					BestTeamId = TestTeamId;
					BestPlayerCount = TestTeamPlayerCount;
				}
			}
		}

		return BestTeamId;
	}

	return INDEX_NONE;
}

void ULRTeamCreationComponent::OnExperienceLoaded(const ULRExperience* Experience)
{
	CreateTeams();
	AssignPlayersToTeams();
}
