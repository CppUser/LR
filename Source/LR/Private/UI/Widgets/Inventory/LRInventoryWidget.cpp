// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Inventory/LRInventoryWidget.h"

#include "CommonUIExtensions.h"
#include "Gameplay/Inventory/LRInventoryManagerComponent.h"
#include "Widgets/UIExtensionPointWidget.h"

ULRInventoryWidget::ULRInventoryWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	InputConfig = EWidgetInputMode::GameAndMenu;

	bAutoActivate = true;
	bSetVisibilityOnActivated = true;
	ActivatedVisibility = ESlateVisibility::Visible;
	DeactivatedVisibility = ESlateVisibility::Collapsed;
}

void ULRInventoryWidget::SetInventoryComponent(ULRInventoryManagerComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
	RefreshInventory();
}

void ULRInventoryWidget::RefreshInventory()
{
}

void ULRInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void ULRInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshInventory();
}

void ULRInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void ULRInventoryWidget::NativeOnDeactivated()
{
	UCommonUIExtensions::PopContentFromLayer(this);
	Super::NativeOnDeactivated();
}

UWidget* ULRInventoryWidget::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

bool ULRInventoryWidget::NativeOnHandleBackAction()
{
	return Super::NativeOnHandleBackAction();
}
