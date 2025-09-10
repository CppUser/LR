// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Equipment/LREquipmentInstance.h"

#include "GameFramework/Character.h"
#include "Gameplay/Equipment/LREquipmentDefinition.h"

ULREquipmentInstance::ULREquipmentInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULREquipmentInstance::Tick(float DeltaTime)
{
}

void ULREquipmentInstance::BeginDestroy()
{
	UObject::BeginDestroy();
}

void ULREquipmentInstance::AddEvolutionProgress(EEquipmentEvolutionTrigger Trigger, float Amount)
{
}

bool ULREquipmentInstance::CanEvolveToNextStage() const
{
}

void ULREquipmentInstance::TriggerEvolution()
{
}

float ULREquipmentInstance::GetEvolutionProgress(EEquipmentEvolutionTrigger Trigger) const
{
}

void ULREquipmentInstance::ModifyMood(float Amount, const FGameplayTag& Reason)
{
}

void ULREquipmentInstance::RememberAction(const FGameplayTag& ActionTag)
{
}

bool ULREquipmentInstance::AttemptActivation()
{
}

void ULREquipmentInstance::TriggerDialogue(const FString& Situation)
{
}

void ULREquipmentInstance::FeedItem(float Amount)
{
}

bool ULREquipmentInstance::IsItemAlive() const
{
}

void ULREquipmentInstance::KillItem()
{
}

float ULREquipmentInstance::GetLifespanRemaining() const
{
}

void ULREquipmentInstance::AttemptIdentification(const FGameplayTag& Method, float Power)
{
}

void ULREquipmentInstance::ForceReveal()
{
}

void ULREquipmentInstance::OnEnvironmentChanged(const FGameplayTag& BiomeTag)
{
}

void ULREquipmentInstance::UpdateResonance(const TArray<ULREquipmentInstance*>& OtherEquipment)
{
}

float ULREquipmentInstance::GetOverallEffectiveness() const
{
}

float ULREquipmentInstance::GetDamageMultiplier() const
{
}

float ULREquipmentInstance::GetDefenseMultiplier() const
{
}

void ULREquipmentInstance::OnKillEnemy(AActor* Enemy, float Damage)
{
}

void ULREquipmentInstance::OnTakeDamage(float Damage, AActor* Attacker)
{
}

void ULREquipmentInstance::OnAbilityUsed(const FGameplayTag& AbilityTag)
{
}

void ULREquipmentInstance::OnPlayerAction(const FGameplayTag& ActionTag)
{
}

void ULREquipmentInstance::UpdateEvolution(float DeltaTime)
{
}

void ULREquipmentInstance::UpdatePersonality(float DeltaTime)
{
}

void ULREquipmentInstance::UpdateLivingItem(float DeltaTime)
{
}

void ULREquipmentInstance::UpdateMystery(float DeltaTime)
{
}

void ULREquipmentInstance::UpdateParasiticEffects(float DeltaTime)
{
}

void ULREquipmentInstance::ApplyEvolutionStage(int32 StageIndex)
{
}

void ULREquipmentInstance::RemoveEvolutionStage(int32 StageIndex)
{
}

void ULREquipmentInstance::ProcessDialogue(const TArray<FText>& PossibleDialogue)
{
}

UWorld* ULREquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

APawn* ULREquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* ULREquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void ULREquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void ULREquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void ULREquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void ULREquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
