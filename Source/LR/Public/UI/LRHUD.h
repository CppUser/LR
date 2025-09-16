// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "GameFramework/HUD.h"
#include "LRHUD.generated.h"


USTRUCT()
struct FHUDLayoutRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=UI)
	TSoftClassPtr<class UCommonActivatableWidget> LayoutClass;

	UPROPERTY(EditAnywhere, Category=UI)
	FGameplayTag LayerID;
};

USTRUCT()
struct FHUDElementEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=UI)
	TSoftClassPtr<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag SlotID;
};


UCLASS()
class LR_API ALRHUD : public AHUD
{
	GENERATED_BODY()
public:
	ALRHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
	void SetupWidgets();
	void RemoveWidgets();
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<FHUDLayoutRequest> DefaultLayouts;
    
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<FHUDElementEntry> DefaultWidgets;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UCommonActivatableWidget>> ActiveLayouts;
    
	TArray<FUIExtensionHandle> ExtensionHandles;
};
