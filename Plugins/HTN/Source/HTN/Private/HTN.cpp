// Fill out your copyright notice in the Description page of Project Settings.


#include "HTN.h"

bool UHTN::ValidateAsset(TArray<FString>& Errors)
{
	bool bIsValid = true;
	if (!BlackboardAsset)
	{
		Errors.Add(TEXT("Missing Blackboard Asset"));
		bIsValid = false;
	}

	//TODO: Add other validations // i.e. Graph nodes etc
    return bIsValid;
}
