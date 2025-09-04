// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Character/LRCharacter.h"


// Sets default values
ALRCharacter::ALRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ALRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

