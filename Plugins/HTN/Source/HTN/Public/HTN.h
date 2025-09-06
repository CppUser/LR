// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

UCLASS(BlueprintType)
class HTN_API UHTN : public UObject
{
	GENERATED_BODY()
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* Graph;
#endif
};

class FHTNModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
