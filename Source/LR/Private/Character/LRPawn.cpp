// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LRPawn.h"


// Sets default values
ALRPawn::ALRPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALRPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ALRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

