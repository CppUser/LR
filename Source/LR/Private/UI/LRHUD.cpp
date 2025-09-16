// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LRHUD.h"

#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"


ALRHUD::ALRHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ALRHUD::BeginPlay()
{
	Super::BeginPlay();
	SetupWidgets();
}

void ALRHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveWidgets();
	Super::EndPlay(EndPlayReason);
}

void ALRHUD::SetupWidgets()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
		return;

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
	if (!LocalPlayer)
		return;

	for (const FHUDLayoutRequest& Layout : DefaultLayouts)
	{
		if (TSubclassOf<UCommonActivatableWidget> WidgetClass = Layout.LayoutClass.Get())
		{
			UCommonActivatableWidget* Widget = UCommonUIExtensions::PushContentToLayer_ForPlayer(
				LocalPlayer, 
				Layout.LayerID, 
				WidgetClass
			);
			ActiveLayouts.Add(Widget);
		}
	}

	if (UUIExtensionSubsystem* ExtensionSubsystem = GetWorld()->GetSubsystem<UUIExtensionSubsystem>())
	{
		for (const FHUDElementEntry& Entry : DefaultWidgets)
		{
			FUIExtensionHandle Handle = ExtensionSubsystem->RegisterExtensionAsWidgetForContext(
				Entry.SlotID, 
				LocalPlayer, 
				Entry.WidgetClass.Get(), 
				-1
			);
			ExtensionHandles.Add(Handle);
		}
	}
}

void ALRHUD::RemoveWidgets()
{
	for (TWeakObjectPtr<UCommonActivatableWidget>& Layout : ActiveLayouts)
	{
		if (Layout.IsValid())
		{
			Layout->DeactivateWidget();
		}
	}
	ActiveLayouts.Empty();

	for (FUIExtensionHandle& Handle : ExtensionHandles)
	{
		Handle.Unregister();
	}
	ExtensionHandles.Empty();
}
