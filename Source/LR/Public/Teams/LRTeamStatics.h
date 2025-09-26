// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LRTeamStatics.generated.h"

class ULRTeamDisplayAsset;
/**
 * 
 */
UCLASS()
class LR_API ULRTeamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category=Teams, meta=(Keywords="GetTeamFromObject", DefaultToSelf="Agent", AdvancedDisplay="bLogIfNotSet"))
	static void FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId, ULRTeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet = false);

	UFUNCTION(BlueprintCallable, Category=Teams, meta=(WorldContext="WorldContextObject"))
	static ULRTeamDisplayAsset* GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static float GetTeamScalarWithFallback(ULRTeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static FLinearColor GetTeamColorWithFallback(ULRTeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static UTexture* GetTeamTextureWithFallback(ULRTeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue);

};
