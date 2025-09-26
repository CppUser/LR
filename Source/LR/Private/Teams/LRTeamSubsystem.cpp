// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/LRTeamSubsystem.h"

#include "AbilitySystemGlobals.h"
#include "GenericTeamAgentInterface.h"
#include "Player/LRPlayerState.h"
#include "Teams/ILRTeamAgentInterface.h"
#include "Teams/LRTeamInfo.h"

/////////////////////////////////////////////////////////
///           FTeamTrackingInfo
/////////////////////////////////////////////////////////
void FTeamTrackingInfo::SetTeamInfo(ALRTeamInfo* Info)
{
	if (ALRTeamInfo* NewInfo = Cast<ALRTeamInfo>(Info))
	{
		ensure((Info == nullptr) || (TeamInfo == NewInfo));
		TeamInfo = NewInfo;

		ULRTeamDisplayAsset* OldDisplayAsset = DisplayAsset;
		DisplayAsset = NewInfo->GetTeamDisplayAsset();

		if (OldDisplayAsset != DisplayAsset)
		{
			OnTeamDisplayAssetChanged.Broadcast(DisplayAsset);
		}
	}
	else
	{
		checkf(false, TEXT("Expected a public or private team info but got %s"), *GetPathNameSafe(Info))
	}
}

void FTeamTrackingInfo::RemoveTeamInfo(ALRTeamInfo* InInfo)
{
	if (TeamInfo == InInfo)
	{
		TeamInfo = nullptr;
	}
	else
	{
		ensureMsgf(false, TEXT("Expected a previously registered team info but got %s"), *GetPathNameSafe(InInfo));
	}
}

/////////////////////////////////////////////////////////
///           ULRTeamSubsystem
/////////////////////////////////////////////////////////
ULRTeamSubsystem::ULRTeamSubsystem()
{
}

void ULRTeamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULRTeamSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool ULRTeamSubsystem::RegisterTeamInfo(ALRTeamInfo* TeamInfo)
{
	if (!ensure(TeamInfo))
	{
		return false;
	}

	const int32 TeamId = TeamInfo->GetTeamId();
	if (ensure(TeamId != INDEX_NONE))
	{
		FTeamTrackingInfo& Entry = TeamMap.FindOrAdd(TeamId);
		Entry.SetTeamInfo(TeamInfo);

		return true;
	}

	return false;
}

bool ULRTeamSubsystem::UnregisterTeamInfo(ALRTeamInfo* TeamInfo)
{
	if (!ensure(TeamInfo))
	{
		return false;
	}

	const int32 TeamId = TeamInfo->GetTeamId();
	if (ensure(TeamId != INDEX_NONE))
	{
		FTeamTrackingInfo* Entry = TeamMap.Find(TeamId);

		// If it couldn't find the entry, this is probably a leftover actor from a previous world, ignore it
		if (Entry)
		{
			Entry->RemoveTeamInfo(TeamInfo);

			return true;
		}
	}

	return false;
}

bool ULRTeamSubsystem::ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId)
{
	const FGenericTeamId NewTeamID = IntegerToGenericTeamId(NewTeamId);
	if (ALRPlayerState* LRPS = const_cast<ALRPlayerState*>(FindPlayerStateFromActor(ActorToChange)))
	{
		LRPS->SetGenericTeamId(NewTeamID);
		return true;
	}
	else if (ILRTeamAgentInterface* TeamActor = Cast<ILRTeamAgentInterface>(ActorToChange))
	{
		TeamActor->SetGenericTeamId(NewTeamID);
		return true;
	}
	else
	{
		return false;
	}
}

int32 ULRTeamSubsystem::FindTeamFromObject(const UObject* TestObject) const
{
	if (const ILRTeamAgentInterface* ObjectWithTeamInterface = Cast<ILRTeamAgentInterface>(TestObject))
	{
		return GenericTeamIdToInteger(ObjectWithTeamInterface->GetGenericTeamId());
	}

	if (const AActor* TestActor = Cast<const AActor>(TestObject))
	{
		// See if the instigator is a team actor
		if (const ILRTeamAgentInterface* InstigatorWithTeamInterface = Cast<ILRTeamAgentInterface>(TestActor->GetInstigator()))
		{
			return GenericTeamIdToInteger(InstigatorWithTeamInterface->GetGenericTeamId());
		}

		// TeamInfo actors don't actually have the team interface, so they need a special case
		if (const ALRTeamInfo* TeamInfo = Cast<ALRTeamInfo>(TestActor))
		{
			return TeamInfo->GetTeamId();
		}

		// Fall back to finding the associated player state
		if (const ALRPlayerState* LRPS = FindPlayerStateFromActor(TestActor))
		{
			return LRPS->GetTeamId();
		}
	}

	return INDEX_NONE;
}

const ALRPlayerState* ULRTeamSubsystem::FindPlayerStateFromActor(const AActor* PossibleTeamActor) const
{
	if (PossibleTeamActor != nullptr)
	{
		if (const APawn* Pawn = Cast<const APawn>(PossibleTeamActor))
		{
			//@TODO: Consider an interface instead or have team actors register with the subsystem and have it maintain a map? (or LWC style)
			if (ALRPlayerState* LRPS = Pawn->GetPlayerState<ALRPlayerState>())
			{
				return LRPS;
			}
		}
		else if (const AController* PC = Cast<const AController>(PossibleTeamActor))
		{
			if (ALRPlayerState* LRPS = Cast<ALRPlayerState>(PC->PlayerState))
			{
				return LRPS;
			}
		}
		else if (const ALRPlayerState* LRPS = Cast<const ALRPlayerState>(PossibleTeamActor))
		{
			return LRPS; 
		}

		// Try the instigator
		// 		if (AActor* Instigator = PossibleTeamActor->GetInstigator())
		// 		{
		// 			if (ensure(Instigator != PossibleTeamActor))
		// 			{
		// 				return FindPlayerStateFromActor(Instigator);
		// 			}
		// 		}
	}

	return nullptr;
}

void ULRTeamSubsystem::FindTeamFromActor(const UObject* TestActor, bool& bIsPartOfTeam, int32& TeamId) const
{
	TeamId = FindTeamFromObject(TestActor);
	bIsPartOfTeam = TeamId != INDEX_NONE;
}

ETeamComparison ULRTeamSubsystem::CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const
{
	TeamIdA = FindTeamFromObject(Cast<const AActor>(A));
	TeamIdB = FindTeamFromObject(Cast<const AActor>(B));

	if ((TeamIdA == INDEX_NONE) || (TeamIdB == INDEX_NONE))
	{
		return ETeamComparison::InvalidArgument;
	}
	else
	{
		return (TeamIdA == TeamIdB) ? ETeamComparison::OnSameTeam : ETeamComparison::DifferentTeams;
	}
}

ETeamComparison ULRTeamSubsystem::CompareTeams(const UObject* A, const UObject* B) const
{
	int32 TeamIdA;
	int32 TeamIdB;
	return CompareTeams(A, B, /*out*/ TeamIdA, /*out*/ TeamIdB);
}

bool ULRTeamSubsystem::CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf) const
{
	if (bAllowDamageToSelf)
	{
		if ((Instigator == Target) || (FindPlayerStateFromActor(Cast<AActor>(Instigator)) == FindPlayerStateFromActor(Cast<AActor>(Target))))
		{
			return true;
		}
	}

	int32 InstigatorTeamId;
	int32 TargetTeamId;
	const ETeamComparison Relationship = CompareTeams(Instigator, Target, /*out*/ InstigatorTeamId, /*out*/ TargetTeamId);
	if (Relationship == ETeamComparison::DifferentTeams)
	{
		return true;
	}
	else if ((Relationship == ETeamComparison::InvalidArgument) && (InstigatorTeamId != INDEX_NONE))
	{
		// Allow damaging non-team actors for now, as long as they have an ability system component
		//@TODO: This is temporary until the target practice dummy has a team assignment
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Cast<const AActor>(Target)) != nullptr;
	}

	return false;
}

void ULRTeamSubsystem::AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("AddTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (FTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->TeamInfo)
		{

			Entry->TeamInfo->TeamTags.AddStack(Tag, StackCount);
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

void ULRTeamSubsystem::RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (FTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->TeamInfo)
		{
			Entry->TeamInfo->TeamTags.RemoveStack(Tag, StackCount);
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

int32 ULRTeamSubsystem::GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const
{
	if (const FTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		const int32 PublicStackCount = (Entry->TeamInfo != nullptr) ? Entry->TeamInfo->TeamTags.GetStackCount(Tag) : 0;
		return PublicStackCount;
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("GetTeamTagStackCount(TeamId: %d, Tag: %s) failed because it was passed an unknown team id"), TeamId, *Tag.ToString());
		return 0;
	}
}

bool ULRTeamSubsystem::TeamHasTag(int32 TeamId, FGameplayTag Tag) const
{
	return GetTeamTagStackCount(TeamId, Tag) > 0;
}

bool ULRTeamSubsystem::DoesTeamExist(int32 TeamId) const
{
	return TeamMap.Contains(TeamId);
}

ULRTeamDisplayAsset* ULRTeamSubsystem::GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId)
{
	if (FTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		return Entry->DisplayAsset;
	}

	return nullptr;
}

ULRTeamDisplayAsset* ULRTeamSubsystem::GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent)
{
	return GetTeamDisplayAsset(TeamId, FindTeamFromObject(ViewerTeamAgent));
}

TArray<int32> ULRTeamSubsystem::GetTeamIDs() const
{
	TArray<int32> Result;
	TeamMap.GenerateKeyArray(Result);
	Result.Sort();
	return Result;
}

void ULRTeamSubsystem::NotifyTeamDisplayAssetModified(ULRTeamDisplayAsset* ModifiedAsset)
{
	for (const auto& KVP : TeamMap)
	{
		const int32 TeamId = KVP.Key;
		const FTeamTrackingInfo& TrackingInfo = KVP.Value;

		TrackingInfo.OnTeamDisplayAssetChanged.Broadcast(TrackingInfo.DisplayAsset);
	}
}

FOnTeamDisplayAssetChangedDelegate& ULRTeamSubsystem::GetTeamDisplayAssetChangedDelegate(int32 TeamId)
{
	return TeamMap.FindOrAdd(TeamId).OnTeamDisplayAssetChanged;
}
