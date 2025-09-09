// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "HTNAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class HTNEDITOR_API UHTNAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UHTNAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool CanCreateNew() const override { return true; }
	virtual bool ShouldShowInNewMenu() const override { return true; }
};
