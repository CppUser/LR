// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNTypes.h"
#include "HTNWorldState.generated.h"


UCLASS(BlueprintType, Blueprintable)
class LR_API UHTNWorldState : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HTN")
	void SetProperty(FName PropertyName, const FHTNWorldProperty& Value);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	FHTNWorldProperty GetProperty(FName PropertyName) const;

	UFUNCTION(BlueprintCallable, Category = "HTN")
	bool HasProperty(FName PropertyName) const;

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void ApplyEffect(const FHTNEffect& Effect);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	UHTNWorldState* Clone() const;

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void CopyFrom(const UHTNWorldState* Other);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	float CalculateDifference(const UHTNWorldState* Other) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FHTNWorldProperty> Properties;

	UPROPERTY(BlueprintReadOnly)
	int32 StateVersion = 0;
};
