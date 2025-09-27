// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "LRTeamCreationComponent.generated.h"


class ALRPlayerState;
class ULRTeamDisplayAsset;
class ALRTeamInfo;
class ULRExperience;

UCLASS(Blueprintable)
class LR_API ULRTeamCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	ULRTeamCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	void SetTeams(TMap<uint8, TObjectPtr<ULRTeamDisplayAsset>> InTeams){TeamsToCreate = InTeams;}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
protected:
	virtual void CreateTeams();
	virtual void AssignPlayersToTeams();
	virtual void ChooseTeamForPlayer(ALRPlayerState* PS);

private:
	void OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer);
	void CreateTeam(int32 TeamId, ULRTeamDisplayAsset* DisplayAsset);
	int32 GetLeastPopulatedTeamID() const;
private:
	void OnExperienceLoaded(const ULRExperience* Experience);

protected:
	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TMap<uint8, TObjectPtr<ULRTeamDisplayAsset>> TeamsToCreate;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<ALRTeamInfo> TeamInfoClass;

};
