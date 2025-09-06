// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstanceProxy.h"
#include "LRAnimInstance.generated.h"

struct FAnimNodeReference;
struct FAnimUpdateContext;
class UCharacterMovementComponent;
class UAbilitySystemComponent;


USTRUCT()
struct FLRAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()
public:

	virtual void InitializeObjects(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	virtual bool Evaluate(FPoseContext& Output) override;

	const FVector& GetWorldLocation() const { return WorldLocation; }
	const float& GetDisplacementSpeed() const {return DisplacementSpeed;}
private:
	void UpdateLocationData(float DeltaTime);
	void UpdateRotationData(float DeltaTime);
	void UpdateVelocityData();
	void UpdateAccelerationData();
	void UpdateWallDetectionHeuristic();
	void UpdateCharacterStateData();
	void UpdateBlendWeightData();
	void UpdateRootYawOffset();
	void UpdateAimingData();
	void UpdateJumpFallData();
private:
	UPROPERTY(Transient)
	APawn* Owner = nullptr;
    
	UPROPERTY(Transient)
	UCharacterMovementComponent* MovementComponent = nullptr;

	FVector ActorLocation{};
	FRotator ActorRotation{};
	FVector ActorVelocity{};
	
	FVector WorldLocation{};
	float DisplacementSpeedSinceLastUpdate = 0.0f;
	float DisplacementSpeed = 0.0f;

	bool bIsFirstUpdate = true;
};

UCLASS(Config = Game)
class LR_API ULRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	ULRAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	virtual void NativeInitializeAnimation() override;


	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	void UpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
protected:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override
	{
		return &Proxy;
	}

	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Location Data", meta = (AllowPrivateAccess = "true"))
	FVector WorldLocation;
    
	UPROPERTY(BlueprintReadOnly, Category = "Location Data", meta = (AllowPrivateAccess = "true"))
	float DisplacementSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(Transient)
	FLRAnimInstanceProxy Proxy;
    
	friend struct FLRAnimInstanceProxy;
};
