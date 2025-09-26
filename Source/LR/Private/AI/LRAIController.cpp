// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRAIController.h"

#include "AbilitySystem/Core/LRAbilitySystemComponent.h"

ALRAIController::ALRAIController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULRAbilitySystemComponent>(this, "ASC");
	AbilitySystemComponent->SetIsReplicated(false);
}

UAbilitySystemComponent* ALRAIController::GetAbilitySystemComponent() const
{
	return GetLRAbilitySystemComponent();
}

void ALRAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(IsValid(AbilitySystemComponent));
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (AbilitySetOnSpawn)
	{
		UE_LOG(LogTemp,Warning, TEXT("Granting OnSpawn Ability Set [%s]"), *GetNameSafe(AbilitySetOnSpawn));

		AbilitySetOnSpawn->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandlesOnSpawn);
	}
}

void ALRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ALRAIController::OnUnPossess()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();
		AbilitySystemComponent->ClearActorInfo();

		GrantedHandlesOnSpawn.TakeFromAbilitySystem(AbilitySystemComponent);
	}
	
	Super::OnUnPossess();
	
}
