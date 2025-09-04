// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularCharacter.h"
#include "LRCharacter.generated.h"

UCLASS()
class LR_API ALRCharacter : public AModularCharacter
{
	GENERATED_BODY()

public:
	ALRCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
