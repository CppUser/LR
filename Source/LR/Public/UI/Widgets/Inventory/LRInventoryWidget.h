// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/LRActivatableWidget.h"
#include "CommonListView.h"
#include "CommonButtonBase.h"
#include "LRInventoryWidget.generated.h"

class UUIExtensionPointWidget;
class ULRInventoryItemInstance;
class ULRInventoryManagerComponent;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class LR_API ULRInventoryWidget : public ULRActivatableWidget
{
	GENERATED_BODY()
public:
	ULRInventoryWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventoryComponent(ULRInventoryManagerComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	virtual bool NativeOnHandleBackAction() override;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<ULRInventoryManagerComponent> InventoryComponent;

};
