// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/LRTeamStatics.h"

#include "Teams/LRTeamDisplayAsset.h"
#include "Teams/LRTeamSubsystem.h"

void ULRTeamStatics::FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId,
                                        ULRTeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet)
{
	bIsPartOfTeam = false;
	TeamId = INDEX_NONE;
	DisplayAsset = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(Agent, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (ULRTeamSubsystem* TeamSubsystem = World->GetSubsystem<ULRTeamSubsystem>())
		{
			TeamId = TeamSubsystem->FindTeamFromObject(Agent);
			if (TeamId != INDEX_NONE)
			{
				bIsPartOfTeam = true;

				DisplayAsset = TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);

				if ((DisplayAsset == nullptr) && bLogIfNotSet)
				{
					UE_LOG(LogTemp, Log, TEXT("FindTeamFromObject(%s) called too early (found team %d but no display asset set yet"), *GetPathNameSafe(Agent), TeamId);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FindTeamFromObject(%s) failed: Team subsystem does not exist yet"), *GetPathNameSafe(Agent));
		}
	}
}

ULRTeamDisplayAsset* ULRTeamStatics::GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId)
{
	ULRTeamDisplayAsset* Result = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (ULRTeamSubsystem* TeamSubsystem = World->GetSubsystem<ULRTeamSubsystem>())
		{
			return TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);
		}
	}
	return Result;
}

float ULRTeamStatics::GetTeamScalarWithFallback(ULRTeamDisplayAsset* DisplayAsset, FName ParameterName,
	float DefaultValue)
{
	if (DisplayAsset)
	{
		if (float* pValue = DisplayAsset->ScalarParameters.Find(ParameterName))
		{
			return *pValue;
		}
	}
	return DefaultValue;
}

FLinearColor ULRTeamStatics::GetTeamColorWithFallback(ULRTeamDisplayAsset* DisplayAsset, FName ParameterName,
	FLinearColor DefaultValue)
{
	if (DisplayAsset)
	{
		if (FLinearColor* pColor = DisplayAsset->ColorParameters.Find(ParameterName))
		{
			return *pColor;
		}
	}
	return DefaultValue;
}

UTexture* ULRTeamStatics::GetTeamTextureWithFallback(ULRTeamDisplayAsset* DisplayAsset, FName ParameterName,
	UTexture* DefaultValue)
{
	if (DisplayAsset)
	{
		if (TObjectPtr<UTexture>* pTexture = DisplayAsset->TextureParameters.Find(ParameterName))
		{
			return *pTexture;
		}
	}
	return DefaultValue;
}
