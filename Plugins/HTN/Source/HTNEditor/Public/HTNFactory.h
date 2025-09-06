// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "HTNFactory.generated.h"


UCLASS()
class HTNEDITOR_API UHTNFactory : public UFactory
{
	GENERATED_BODY()
public:
	UHTNFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
