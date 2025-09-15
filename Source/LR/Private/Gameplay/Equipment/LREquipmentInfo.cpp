// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Equipment/LREquipmentInfo.h"

#include "Gameplay/Equipment/LREquipmentInstance.h"

ULREquipmentInfo::ULREquipmentInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstanceType = ULREquipmentInstance::StaticClass();
}
