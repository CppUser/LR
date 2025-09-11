// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/Tasks/LRScanForInteractables.h"

#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Gameplay/Interaction/LRInteractableTarget.h"
#include "Gameplay/Interaction/LRInteractionOption.h"
#include "Gameplay/Interaction/LRInteractionQuery.h"
#include "Gameplay/Interaction/LRInteractionStatics.h"
#include "Physics/LRCollisionChannels.h"

ULRScanForInteractables::ULRScanForInteractables(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

ULRScanForInteractables* ULRScanForInteractables::ScanForInteractables(UGameplayAbility* OwningAbility, float InScanRange,
	float InScanRate, float InScanAngle, bool bInRequireLineOfSight, bool bInScanInCone, EInteractionMethod InMethod)
{
	ULRScanForInteractables* MyTask = NewAbilityTask<ULRScanForInteractables>(OwningAbility);
	MyTask->ScanRange = InScanRange;
	MyTask->ScanRate = InScanRate;
	MyTask->ScanAngle = InScanAngle;
	MyTask->bRequireLineOfSight = bInRequireLineOfSight;
	MyTask->bScanInCone = bInScanInCone;
	MyTask->InteractionMethod = InMethod;
	return MyTask;
}

void ULRScanForInteractables::Activate()
{
	SetWaitingOnAvatar();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ScanTimerHandle, this, 
			&ULRScanForInteractables::PerformScan, ScanRate, true);
        
		// Perform initial scan immediately
		PerformScan();
	}
}

void ULRScanForInteractables::OnDestroy(bool AbilityEnded)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScanTimerHandle);
	}

	//TODO: Pull acs properly
	// Clean up granted abilities 
	// for (const auto& Pair : GrantedAbilityCache)
	// {
	// 	AbilitySystemComponent->ClearAbility(Pair.Value);
	// }
	// GrantedAbilityCache.Empty();
	//
	Super::OnDestroy(AbilityEnded);
}

void ULRScanForInteractables::PerformScan()
{
	switch (InteractionMethod)
	{
	case EInteractionMethod::Proximity:
		ScanProximity();
		break;
	case EInteractionMethod::LineTrace:
		ScanLineTrace();
		break;
	case EInteractionMethod::MouseOver:
		ScanMouseOver();
		break;
	case EInteractionMethod::Combined:
		ScanProximity();
		ScanLineTrace();
		break;
	}
}

void ULRScanForInteractables::ScanProximity()
{
	AActor* Avatar = GetAvatarActor();
	UWorld* World = GetWorld();
	if (!Avatar || !World) return;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ULRTask_ScanForInteractables), false);
	Params.AddIgnoredActor(Avatar);

	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByChannel(
		OverlapResults,
		Avatar->GetActorLocation(),
		FQuat::Identity,
		LR_TraceChannel_Interaction,
		FCollisionShape::MakeSphere(ScanRange),
		Params
	);

	TArray<TScriptInterface<ILRInteractableTarget>> InteractableTargets;
    
	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* OverlapActor = Result.GetActor();
		if (!OverlapActor) continue;

		// Check if in scan area (cone or full circle)
		if (!IsInScanArea(OverlapActor)) continue;

		// Check line of sight if required
		if (bRequireLineOfSight && !CheckLineOfSight(OverlapActor)) continue;

		ULRInteractionStatics::AppendInteractableTargetsFromOverlapResults(
			TArray<FOverlapResult>{Result}, InteractableTargets);
	}

	ProcessInteractableTargets(InteractableTargets);
}

void ULRScanForInteractables::ScanLineTrace()
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	APlayerController* PC = nullptr;
	if (Ability)
	{
		PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	}
	if (!PC) return;

	FVector ViewStart, TraceEnd;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	// Calculate trace end based on view direction
	FVector ViewDir = ViewRot.Vector();
	TraceEnd = ViewStart + (ViewDir * ScanRange);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ULRTask_ScanForInteractables), false);
	Params.AddIgnoredActor(Avatar);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ViewStart,
		TraceEnd,
		LR_TraceChannel_Interaction,
		Params
	);

	if (HitResult.bBlockingHit)
	{
		TArray<TScriptInterface<ILRInteractableTarget>> InteractableTargets;
		ULRInteractionStatics::AppendInteractableTargetsFromHitResult(HitResult, InteractableTargets);
        
		if (InteractableTargets.Num() > 0)
		{
			ProcessInteractableTargets(InteractableTargets);
		}
	}
}

void ULRScanForInteractables::ScanMouseOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Mouse over scan not implemented"));
}

bool ULRScanForInteractables::CheckLineOfSight(const AActor* Target) const
{
	if (!Target || !bScanInCone) return true;

	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return false;

	FVector ToTarget = Target->GetActorLocation() - Avatar->GetActorLocation();
	ToTarget.Z = 0;
	ToTarget.Normalize();

	FVector Forward = Avatar->GetActorForwardVector();
	Forward.Z = 0;
	Forward.Normalize();

	float DotProduct = FVector::DotProduct(Forward, ToTarget);
	float AngleRadians = FMath::Acos(DotProduct);
	float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	return AngleDegrees <= (ScanAngle * 0.5f);
}

bool ULRScanForInteractables::IsInScanArea(const AActor* Target) const
{
	if (!Target) return false;

	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(CheckLineOfSight), false);
	Params.AddIgnoredActor(Avatar);
	Params.AddIgnoredActor(Target);

	FHitResult HitResult;
	return !GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Avatar->GetActorLocation() + FVector(0, 0, 50), // Slightly elevated to avoid ground
		Target->GetActorLocation() + FVector(0, 0, 50),
		ECC_Visibility,
		Params
	);
}

void ULRScanForInteractables::ProcessInteractableTargets(const TArray<TScriptInterface<ILRInteractableTarget>>& Targets)
{
	AActor* Avatar = GetAvatarActor();
    if (!Avatar) return;

    FLRInteractionQuery Query;
    Query.RequestingAvatar = Avatar;
    Query.RequestingController = Cast<AController>(Avatar->GetOwner());

    TArray<FLRInteractionOption> AllOptions;

    for (const auto& Target : Targets)
    {
        TArray<FLRInteractionOption> BaseOptions;
        FLRInteractionOptionBuilder Builder(Target, BaseOptions,Query);
        Target->GatherInteractionOptions(Query, Builder);

        // Convert to enhanced options and check requirements
        for (const FLRInteractionOption& BaseOption : BaseOptions)
        {
            FLRInteractionOption EnhancedOption;
            // Copy base fields
            EnhancedOption.InteractableTarget = BaseOption.InteractableTarget;
            EnhancedOption.Text = BaseOption.Text;
            EnhancedOption.SubText = BaseOption.SubText;
            EnhancedOption.InteractionAbilityToGrant = BaseOption.InteractionAbilityToGrant;
            EnhancedOption.TargetAbilitySystem = BaseOption.TargetAbilitySystem;
            EnhancedOption.TargetInteractionAbilityHandle = BaseOption.TargetInteractionAbilityHandle;
            EnhancedOption.InteractionWidgetClass = BaseOption.InteractionWidgetClass;

            // Check requirements
            AActor* TargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(Target);
            if (EnhancedOption.Requirements.CheckRequirements(Avatar, TargetActor))
            {
            	if (UAbilitySystemComponent* ASC = EnhancedOption.TargetAbilitySystem.Get())
            	{
            		// Grant ability if needed
            		if (EnhancedOption.InteractionAbilityToGrant && ASC)
            		{
            			FObjectKey ObjectKey(EnhancedOption.InteractionAbilityToGrant);
            			if (!GrantedAbilityCache.Contains(ObjectKey))
            			{
            				FGameplayAbilitySpec Spec(EnhancedOption.InteractionAbilityToGrant, 1, INDEX_NONE, this);
            				FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
            				GrantedAbilityCache.Add(ObjectKey, Handle);
            			}
            		}
            	}
                

                AllOptions.Add(EnhancedOption);
            }
        }
    }

    UpdateCachedOptions(AllOptions);
}

void ULRScanForInteractables::UpdateCachedOptions(const TArray<FLRInteractionOption>& NewOptions)
{
	TArray<FLRInteractionOption> SortedOptions = NewOptions;
    
	AActor* Avatar = GetAvatarActor();
	if (Avatar)
	{
		SortedOptions.Sort([Avatar](const FLRInteractionOption& A, const FLRInteractionOption& B)
		{
			// First sort by priority
			if (A.Priority != B.Priority)
			{
				return A.Priority > B.Priority;
			}

			// Then by distance
			AActor* ActorA = ULRInteractionStatics::GetActorFromInteractableTarget(A.InteractableTarget);
			AActor* ActorB = ULRInteractionStatics::GetActorFromInteractableTarget(B.InteractableTarget);
            
			if (ActorA && ActorB)
			{
				float DistA = FVector::Dist(Avatar->GetActorLocation(), ActorA->GetActorLocation());
				float DistB = FVector::Dist(Avatar->GetActorLocation(), ActorB->GetActorLocation());
				return DistA < DistB;
			}

			return false;
		});
	}

	// Check if options have changed
	bool bOptionsChanged = SortedOptions.Num() != CachedOptions.Num();
	if (!bOptionsChanged)
	{
		for (int32 i = 0; i < SortedOptions.Num(); i++)
		{
			if (SortedOptions[i] != CachedOptions[i])
			{
				bOptionsChanged = true;
				break;
			}
		}
	}

	if (bOptionsChanged)
	{
		CachedOptions = SortedOptions;
		OnInteractablesFound.Broadcast(CachedOptions);
	}
}
