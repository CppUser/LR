// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ModularPlayerController.h"

#include "CommonPlayerController.generated.h"

/**
 * 
 */
UCLASS(config=Game)
class COMMONGAME_API ACommonPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	ACommonPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ReceivedPlayer() override;	
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(class APawn* APawn) override;
	virtual void OnUnPossess() override;
	
protected:
	virtual void OnRep_PlayerState() override;
};
