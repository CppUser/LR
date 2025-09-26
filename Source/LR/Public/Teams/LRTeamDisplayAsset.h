// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LRTeamDisplayAsset.generated.h"

class UNiagaraComponent;


//TODO: Not Sure if I need it
UCLASS(BlueprintType)
class LR_API ULRTeamDisplayAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category=Teams)
	void ApplyToMaterial(UMaterialInstanceDynamic* Material);

	UFUNCTION(BlueprintCallable, Category=Teams)
	void ApplyToMeshComponent(UMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category=Teams)
	void ApplyToNiagaraComponent(UNiagaraComponent* NiagaraComponent);

	UFUNCTION(BlueprintCallable, Category=Teams, meta=(DefaultToSelf="TargetActor"))
	void ApplyToActor(AActor* TargetActor, bool bIncludeChildActors = true);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, float> ScalarParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FLinearColor> ColorParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, TObjectPtr<UTexture>> TextureParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TeamShortName;
};
