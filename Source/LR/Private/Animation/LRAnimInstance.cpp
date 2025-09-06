// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/LRAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "KismetAnimationLibrary.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

///////////////////////////////////////////////////////////
///              FLRAnimInstanceProxy
///////////////////////////////////////////////////////////



void FLRAnimInstanceProxy::InitializeObjects(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::InitializeObjects(InAnimInstance);

	Owner = InAnimInstance->TryGetPawnOwner();
	if (Owner == nullptr){return;}

	MovementComponent = Cast<UCharacterMovementComponent>(Owner->GetMovementComponent());
}

void FLRAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (Owner)
	{
		ActorLocation = Owner->GetActorLocation();
		ActorRotation = Owner->GetActorRotation();
		ActorVelocity = Owner->GetVelocity();
	}
}

void FLRAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);

	UpdateLocationData(DeltaSeconds);
	
	bIsFirstUpdate = false;
}

bool FLRAnimInstanceProxy::Evaluate(FPoseContext& Output)
{
	return FAnimInstanceProxy::Evaluate(Output);
}

void FLRAnimInstanceProxy::UpdateLocationData(float DeltaTime)
{
	DisplacementSpeedSinceLastUpdate = FVector::DistXY(ActorLocation, WorldLocation);
	WorldLocation = ActorLocation;
	DisplacementSpeed = DeltaTime > 0.0f ? DisplacementSpeedSinceLastUpdate / DeltaTime : 0.0f;
    
	if (bIsFirstUpdate)
	{
		DisplacementSpeedSinceLastUpdate = 0.0f;
		DisplacementSpeed = 0.0f;
	}
}

void FLRAnimInstanceProxy::UpdateRotationData(float DeltaTime)
{
}

void FLRAnimInstanceProxy::UpdateVelocityData()
{
}

void FLRAnimInstanceProxy::UpdateAccelerationData()
{
}

void FLRAnimInstanceProxy::UpdateWallDetectionHeuristic()
{
}

void FLRAnimInstanceProxy::UpdateCharacterStateData()
{
}

void FLRAnimInstanceProxy::UpdateBlendWeightData()
{
}

void FLRAnimInstanceProxy::UpdateRootYawOffset()
{
}

void FLRAnimInstanceProxy::UpdateAimingData()
{
}

void FLRAnimInstanceProxy::UpdateJumpFallData()
{
}

///////////////////////////////////////////////////////////
///               ULRAnimInstance
///////////////////////////////////////////////////////////




ULRAnimInstance::ULRAnimInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void ULRAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);
	GameplayTagPropertyMap.Initialize(this, ASC);
}



#if WITH_EDITOR
EDataValidationResult ULRAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif

void ULRAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void ULRAnimInstance::UpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
}

void ULRAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	
	WorldLocation = Proxy.GetWorldLocation();
	DisplacementSpeed = Proxy.GetDisplacementSpeed();

	// UE_LOG(LogTemp,Warning,TEXT("WorldLocation : X: %f , Y: %f , Z: %f"),WorldLocation.X,WorldLocation.Y,WorldLocation.Z);
	// UE_LOG(LogTemp,Warning,TEXT("DsiplacementSpeed : %f"),DisplacementSpeed);

}
