// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "LRTeamSubsystem.generated.h"

class ALRPlayerState;
class ALRTeamInfo;
class ULRTeamDisplayAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamDisplayAssetChangedDelegate, const ULRTeamDisplayAsset*, DisplayAsset);

USTRUCT()
struct FTeamTrackingInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<ALRTeamInfo> TeamInfo = nullptr;
	
	UPROPERTY()
	TObjectPtr<ULRTeamDisplayAsset> DisplayAsset = nullptr;

	UPROPERTY()
	FOnTeamDisplayAssetChangedDelegate OnTeamDisplayAssetChanged;

public:
	void SetTeamInfo(ALRTeamInfo* Info);
	void RemoveTeamInfo(ALRTeamInfo* InInfo);
};

UENUM(BlueprintType)
enum class ETeamComparison : uint8
{
	OnSameTeam,
	DifferentTeams,
	InvalidArgument
};

UCLASS()
class LR_API ULRTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	ULRTeamSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool RegisterTeamInfo(ALRTeamInfo* TeamInfo);
	bool UnregisterTeamInfo(ALRTeamInfo* TeamInfo);

	bool ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId);

	int32 FindTeamFromObject(const UObject* TestObject) const;

	const ALRPlayerState* FindPlayerStateFromActor(const AActor* PossibleTeamActor) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Teams, meta=(Keywords="Get"))
	void FindTeamFromActor(const UObject* TestActor, bool& bIsPartOfTeam, int32& TeamId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Teams, meta=(ExpandEnumAsExecs=ReturnValue))
	ETeamComparison CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const;
	ETeamComparison CompareTeams(const UObject* A, const UObject* B) const;

	bool CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf = true) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Teams)
	bool TeamHasTag(int32 TeamId, FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Teams)
	bool DoesTeamExist(int32 TeamId) const;

	UFUNCTION(BlueprintCallable, Category=Teams)
	ULRTeamDisplayAsset* GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId);

	UFUNCTION(BlueprintCallable, Category = Teams)
	ULRTeamDisplayAsset* GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Teams)
	TArray<int32> GetTeamIDs() const;

	void NotifyTeamDisplayAssetModified(ULRTeamDisplayAsset* ModifiedAsset);

	FOnTeamDisplayAssetChangedDelegate& GetTeamDisplayAssetChangedDelegate(int32 TeamId);
private:
	UPROPERTY()
	TMap<int32, FTeamTrackingInfo> TeamMap;
};
