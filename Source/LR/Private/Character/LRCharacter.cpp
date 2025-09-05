// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Character/LRCharacter.h"

#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Character/Components/LRPawnExtComponent.h"
#include "Components/CapsuleComponent.h"
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
	//TODO: implement 
	Super::NotifyControllerChanged();
}

void ALRCharacter::OnAbilitySystemInitialized()
{
	ULRAbilitySystemComponent* ASC = GetLRAbilitySystemComponent();
	check(ASC);

	//TODO:HealthComponent->InitializeWithAbilitySystem(ASC);
	//TODO:InitializeGameplayTags();
}

void ALRCharacter::OnAbilitySystemUninitialized()
{
	//TODO:HealthComponent->UninitializeFromAbilitySystem();
}

void ALRCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PawnExtComponent->HandleControllerChanged();
}

void ALRCharacter::UnPossessed()
{
	Super::UnPossessed();
	PawnExtComponent->HandleControllerChanged();
}

void ALRCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnExtComponent->SetupPlayerInputComponent();
}
