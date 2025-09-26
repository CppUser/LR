// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/LRTeamInfo.h"

#include "Teams/LRTeamSubsystem.h"

ALRTeamInfo::ALRTeamInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, TeamId(INDEX_NONE)
{
}

void ALRTeamInfo::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void ALRTeamInfo::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		ULRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<ULRTeamSubsystem>();
		if (TeamSubsystem)
		{
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void ALRTeamInfo::SetTeamId(int32 NewTeamId)
{
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void ALRTeamInfo::RegisterWithTeamSubsystem(ULRTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void ALRTeamInfo::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		ULRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<ULRTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void ALRTeamInfo::SetTeamDisplayAsset(TObjectPtr<ULRTeamDisplayAsset> NewDisplayAsset)
{
	check(TeamDisplayAsset == nullptr);
	TeamDisplayAsset = NewDisplayAsset;
	TryRegisterWithTeamSubsystem();
}
