// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/LRGameplayAbility.h"
#include "LRGA_OpenInventory.generated.h"

/**
 * 
 */
UCLASS()
class LR_API ULRGA_OpenInventory : public ULRGameplayAbility
{
	GENERATED_BODY()
public:
	ULRGA_OpenInventory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "LR|Inventory")
	void ToggleInventory();
	UFUNCTION(BlueprintCallable, Category = "LR|Inventory")
	void OpenInventory();
	UFUNCTION(BlueprintCallable, Category = "LR|Inventory")
	void CloseInventory();

	UFUNCTION(BlueprintPure, Category = "LR|Inventory")
	bool IsInventoryOpen() const {return bIsInventoryOpen;}
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Inventory")
	void OnInventoryOpened();

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Inventory")
	void OnInventoryClosed();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	bool bCloseOnInputAgain = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	bool bPauseGameWhenOpen = false;
private:
	bool bIsInventoryOpen = false;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentInventoryWidget;
	
#pragma region TEMP Inventory Widget
	void CreateInventoryWidget();
	void RemoveInventoryWidget();
#pragma endregion
};
