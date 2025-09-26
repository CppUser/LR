// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Character/LRCharacter.h"

#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Character/Components/LRPawnExtComponent.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/Systems/Equipment/LREquipmentManagerComponent.h"
#include "Gameplay/Systems/Inventory/LRInventoryManagerComponent.h"
#include "Player/LRPlayerController.h"
#include "Player/LRPlayerState.h"

static FName NAME_LRCharacterCollisionProfile_Capsule(TEXT("LRPawnCapsule"));
static FName NAME_LRCharacterCollisionProfile_Mesh(TEXT("LRPawnMesh"));

ALRCharacter::ALRCharacter(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_LRCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); 
	MeshComp->SetCollisionProfileName(NAME_LRCharacterCollisionProfile_Mesh);

	PawnExtComponent = CreateDefaultSubobject<ULRPawnExtComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	EquipmentManagerComponent = CreateDefaultSubobject<ULREquipmentManagerComponent>("EquipmentManagerComponent");
	InventoryManagerComponent = CreateDefaultSubobject<ULRInventoryManagerComponent>("InventoryManagerComponent");
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

ALRPlayerController* ALRCharacter::GetLRPlayerController() const
{
	return CastChecked<ALRPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ALRPlayerState* ALRCharacter::GetLRPlayerState() const
{
	return CastChecked<ALRPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

ULRAbilitySystemComponent* ALRCharacter::GetLRAbilitySystemComponent() const
{
	return Cast<ULRAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ALRCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetLRAbilitySystemComponent();
}

void ALRCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALRCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALRCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALRCharacter::Reset()
{
	Super::Reset();
}

void ALRCharacter::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	if (Controller != nullptr)
	{
		if (ILRTeamAgentInterface* ControllerWithTeam = Cast<ILRTeamAgentInterface>(Controller))
		{
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
		}
	}
}

void ALRCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const ULRAbilitySystemComponent* LRASC = GetLRAbilitySystemComponent())
	{
		LRASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ALRCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const ULRAbilitySystemComponent* LRASC = GetLRAbilitySystemComponent())
	{
		return LRASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ALRCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULRAbilitySystemComponent* LRASC = GetLRAbilitySystemComponent())
	{
		return LRASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ALRCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULRAbilitySystemComponent* LRASC = GetLRAbilitySystemComponent())
	{
		return LRASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ALRCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		const FGenericTeamId OldTeamID = MyTeamID;
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId ALRCharacter::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnTeamIndexChangedDelegate* ALRCharacter::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ALRCharacter::InitializeGameplayTags()
{
	// if (ULRAbilitySystemComponent* LRASC = GetLRAbilitySystemComponent())
	// {
	// 	for (const TPair<uint8, FGameplayTag>& TagMapping : LRGameplayTags::MovementModeTagMap)
	// 	{
	// 		if (TagMapping.Value.IsValid())
	// 		{
	// 			LRASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
	// 		}
	// 	}
	//
	// 	for (const TPair<uint8, FGameplayTag>& TagMapping : LRGameplayTags::CustomMovementModeTagMap)
	// 	{
	// 		if (TagMapping.Value.IsValid())
	// 		{
	// 			LRASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
	// 		}
	// 	}
	//
	// 	// ULRCharacterMovementComponent* LRMoveComp = CastChecked<ULRCharacterMovementComponent>(GetCharacterMovement());
	// 	//TODO: SetMovementModeTag(LRMoveComp->MovementMode, LRMoveComp->CustomMovementMode, true);
	//}
}

void ALRCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void ALRCharacter::OnAbilitySystemInitialized()
{
	ULRAbilitySystemComponent* ASC = GetLRAbilitySystemComponent();
	check(ASC);

	//TODO:HealthComponent->InitializeWithAbilitySystem(ASC);

	InitializeGameplayTags();
}

void ALRCharacter::OnAbilitySystemUninitialized()
{
	//TODO:HealthComponent->UninitializeFromAbilitySystem();
}

void ALRCharacter::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);
	
	PawnExtComponent->HandleControllerChanged();

	if (ILRTeamAgentInterface* ControllerAsTeamProvider = Cast<ILRTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ALRCharacter::UnPossessed()
{
	AController* const OldController = Controller;

	const FGenericTeamId OldTeamID = MyTeamID;
	if (ILRTeamAgentInterface* ControllerAsTeamProvider = Cast<ILRTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}
	
	Super::UnPossessed();
	PawnExtComponent->HandleControllerChanged();

	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ALRCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnExtComponent->SetupPlayerInputComponent();
}
