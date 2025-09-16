// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/LRGameplayAbility.h"
#include "LRGA_OpenInventory.generated.h"

class ULRInventoryWidget;
/**
 * 
 */
UCLASS()
class LR_API ULRGA_OpenInventory : public ULRGameplayAbility
{
	GENERATED_BODY()
public:
	ULRGA_OpenInventory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
        
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "LR|Inventory")
	void OpenInventory();
    
	UFUNCTION(BlueprintCallable, Category = "LR|Inventory")
	void CloseInventory();

	UFUNCTION()
	void OnInventoryWidgetDeactivated();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULRInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (Categories = "UI.Layer"))
	FGameplayTag InventoryLayer = FGameplayTag::RequestGameplayTag("UI.Layer.Menu");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	bool bCloseOnInputAgain = true;
private:
	UPROPERTY()
	TWeakObjectPtr<ULRInventoryWidget> CurrentInventoryWidget;

	bool bIsInventoryOpen = false;
};
