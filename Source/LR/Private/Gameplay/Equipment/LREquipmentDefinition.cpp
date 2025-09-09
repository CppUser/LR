// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Equipment/LREquipmentDefinition.h"
#include "Gameplay/Equipment/LREquipmentInstance.h"

ULREquipmentDefinition::ULREquipmentDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstanceType = ULREquipmentInstance::StaticClass();
}
