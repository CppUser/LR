// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Equipment/LREquipmentInfo.h"

#include "Gameplay/Systems/Equipment/LREquipmentInstance.h"

ULREquipmentInfo::ULREquipmentInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstanceType = ULREquipmentInstance::StaticClass();
}
