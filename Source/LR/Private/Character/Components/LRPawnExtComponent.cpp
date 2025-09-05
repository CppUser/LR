// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Character/Components/LRPawnExtComponent.h"

#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Character/LRPawnData.h"
#include "Utils/LRGameplayTags.h"

const FName ULRPawnExtComponent::NAME_ActorFeatureName("PawnExtension");

ULRPawnExtComponent::ULRPawnExtComponent(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	PawnData = nullptr;
	ASC = nullptr;
}

void ULRPawnExtComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("ULRPawnExtComponent: on [%s] con only be added to Pawn actors"),*GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnComponents;
	Pawn->GetComponents(ULRPawnExtComponent::StaticClass(),PawnComponents);
	ensureAlwaysMsgf((PawnComponents.Num() == 1), TEXT("Only one ULRPawnExtComponent should exist on [%s]."),*GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void ULRPawnExtComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None,FGameplayTag(),false);

	ensure(TryToChangeInitState(LRGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void ULRPawnExtComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void ULRPawnExtComponent::SetPawnData(const ULRPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (PawnData)
	{
		UE_LOG(LogTemp,Error,TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]"), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData))
		return;
	}
	PawnData = InPawnData;
	CheckDefaultInitialization();
}

bool ULRPawnExtComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	
	if (!CurrentState.IsValid() && DesiredState == LRGameplayTags::InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	if (CurrentState == LRGameplayTags::InitState_Spawned && DesiredState == LRGameplayTags::InitState_DataAvailable)
	{
		if (!PawnData)
		{
			return false;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}
		
		return true;
	}
	else if (CurrentState == LRGameplayTags::InitState_DataAvailable && DesiredState == LRGameplayTags::InitState_DataInitialized)
	{
		return Manager->HaveAllFeaturesReachedInitState(Pawn,LRGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == LRGameplayTags::InitState_DataInitialized && DesiredState == LRGameplayTags::InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void ULRPawnExtComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	// This is currently all handled by other components listening to this state change
}

void ULRPawnExtComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == LRGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void ULRPawnExtComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();
	
	static const TArray<FGameplayTag> StateChain = {LRGameplayTags::InitState_Spawned,LRGameplayTags::InitState_DataAvailable,LRGameplayTags::InitState_DataInitialized,LRGameplayTags::InitState_GameplayReady};
	
	ContinueInitStateChain(StateChain);
}

void ULRPawnExtComponent::InitializeAbilitySystem(ULRAbilitySystemComponent* InASC, AActor* OwnerActor)
{
	check(InASC);
	check(OwnerActor);

	if (ASC == InASC)
	{
		return;
	}

	if (ASC)
	{
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogTemp, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(OwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogTemp, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (ULRPawnExtComponent* OtherExtensionComponent = FindPawnExtComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	ASC = InASC;
	ASC->InitAbilityActorInfo(OwnerActor, Pawn);

	//TODO: SetTagRelationshipMapping
	// if (ensure(PawnData))
	// {
	// 	InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	// }

	OnAbilitySystemInitialized.Broadcast();
}

void ULRPawnExtComponent::UninitializeAbilitySystem()
{
	if (!ASC)
	{
		return;
	}

	if (ASC->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(LRGameplayTags::Ability_Behavior_SurvivesDeath);

		ASC->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		ASC->ClearAbilityInput();
		ASC->RemoveAllGameplayCues();

		if (ASC->GetOwnerActor() != nullptr)
		{
			ASC->SetAvatarActor(nullptr);
		}
		else
		{
			ASC->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	ASC = nullptr;
}

void ULRPawnExtComponent::HandleControllerChanged()
{
	if (ASC && (ASC->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(ASC->AbilityActorInfo->OwnerActor == ASC->GetOwnerActor());
		if (ASC->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			ASC->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();
}

void ULRPawnExtComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void ULRPawnExtComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (ASC)
	{
		Delegate.Execute();
	}
}

void ULRPawnExtComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}


