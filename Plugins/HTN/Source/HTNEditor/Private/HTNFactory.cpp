// Fill out your copyright notice in the Description page of Project Settings.


#include "HTNFactory.h"

#include "HTN.h"

UHTNFactory::UHTNFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UHTN::StaticClass();
}

UObject* UHTNFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                       UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UHTN>(InParent, Class, Name, Flags);
}
