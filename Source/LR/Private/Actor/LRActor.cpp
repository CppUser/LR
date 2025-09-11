// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LRActor.h"

//////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////
ALRActor::ALRActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ALRActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(StaticGameplayTags);
}
#if WITH_EDITOR
bool ALRActor::CanEditChange(const FProperty* InProperty) const
{
	// Prevent editing of the other tags property
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AActor, Tags))
	{
		return false;
	}
	
	return Super::CanEditChange(InProperty);
}
#endif
void ALRActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////
///             ALRInteractableActor
/////////////////////////////////////////////////////////
ALRInteractableActor::ALRInteractableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ALRInteractableActor::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(Option);
}

