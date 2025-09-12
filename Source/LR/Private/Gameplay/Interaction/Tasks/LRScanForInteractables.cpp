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
	float InScanRate, float InScanAngle, bool bInRequireLineOfSight, bool bInScanInCone, EInteractionMethod InMethod, bool bShowDebugInfo)
{
	ULRScanForInteractables* MyTask = NewAbilityTask<ULRScanForInteractables>(OwningAbility);
	MyTask->ScanRange = InScanRange;
	MyTask->ScanRate = InScanRate;
	MyTask->ScanAngle = InScanAngle;
	MyTask->bRequireLineOfSight = bInRequireLineOfSight;
	MyTask->bScanInCone = bInScanInCone;
	MyTask->InteractionMethod = InMethod;
	MyTask->bShowDebugInfo = bShowDebugInfo;
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

	// if (bShowDebugInfo)
	// {
	// 	AActor* Avatar = Ability ? Ability->GetCurrentActorInfo()->AvatarActor.Get() : nullptr;
	// 	if (!Avatar) return;
	//
	// 	UWorld* World = Avatar->GetWorld();
	// 	if (!World) return;
	//
	// 	FVector Location = Avatar->GetActorLocation();
	// 	DrawDebugSphere(World, Location, ScanRange, 32, FColor::Yellow, false, 0.1f);
	//
	// 	if (ScanAngle < 180.0f)
	// 	{
	// 		TArray<AActor*> ActorsToIgnore;
	// 		ActorsToIgnore.Add(Avatar);
	//
	// 		const bool bTraceComplex = false;
	// 		FCollisionQueryParams Params(SCENE_QUERY_STAT(ULRScanForInteractables), bTraceComplex);
	// 		Params.AddIgnoredActors(ActorsToIgnore);
	// 		//TODO: Aim with controller
	// 		
	// 		FVector TraceEnd;
	// 		AimWithPlayerController(Avatar, Params, Location, ScanRange, OUT TraceEnd,false);
	//
	//
	// 		FHitResult OutHitResult;
	// 		LineTrace(OutHitResult, World, Location, TraceEnd, TraceProfile.Name, Params);
	//
	// 		
	// 		FVector Forward = Avatar->GetActorForwardVector();
	// 		float HalfAngleRad = FMath::DegreesToRadians(ScanAngle * 0.5f);
 //        
	// 		// Draw cone lines
	// 		FVector LeftDir = Forward.RotateAngleAxis(-ScanAngle * 0.5f, FVector::UpVector);
	// 		FVector RightDir = Forward.RotateAngleAxis(ScanAngle * 0.5f, FVector::UpVector);
 //        
	// 		DrawDebugLine(World, Location, Location + LeftDir * ScanRange, FColor::Green, false, 0.1f, 0, 2.0f);
	// 		DrawDebugLine(World, Location, Location + RightDir * ScanRange, FColor::Green, false, 0.1f, 0, 2.0f);
	// 		DrawDebugLine(World, Location, OutHitResult.Location + Forward * ScanRange, FColor::Red, false, 0.1f, 0, 2.0f);
	// 	}
	// }
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
            	
            	// Grant ability if needed
            	if (EnhancedOption.InteractionAbilityToGrant && AbilitySystemComponent.Get())
            	{
            		FObjectKey ObjectKey(EnhancedOption.InteractionAbilityToGrant);
            		if (!GrantedAbilityCache.Contains(ObjectKey))
            		{
            			FGameplayAbilitySpec Spec(EnhancedOption.InteractionAbilityToGrant, 1, INDEX_NONE, this);
            			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
            			GrantedAbilityCache.Add(ObjectKey, Handle);
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

void ULRScanForInteractables::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params,
	const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch) const
{
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector ViewStart;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange);

	ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	FHitResult HitResult;
	LineTrace(HitResult, InSourceActor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, Params);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = (bUseTraceResult) ? HitResult.Location : ViewEnd;
	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDir;
	}

	if (!bTraceAffectsAimPitch && bUseTraceResult)
	{
		FVector OriginalAimDir = (ViewEnd - TraceStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			// Convert to angles and use original pitch
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool ULRScanForInteractables::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection,
	FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition) const
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	if (DotToCenter >= 0)		//If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;						//Subtracting instead of adding will get the other intersection point
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);		//Cam aim point clipped to range sphere
			return true;
		}
	}
	return false;
}

void ULRScanForInteractables::LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start,
	const FVector& End, FName ProfileName, const FCollisionQueryParams Params) const
{
	check(World);

	OutHitResult = FHitResult();
	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
}
