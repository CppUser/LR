// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Systems/Interaction/Tasks/LRScanForInteractables.h"

#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Gameplay/Systems/Interaction/LRInteractableTarget.h"
#include "Gameplay/Systems/Interaction/LRInteractionOption.h"
#include "Gameplay/Systems/Interaction/LRInteractionQuery.h"
#include "Gameplay/Systems/Interaction/LRInteractionStatics.h"
#include "Physics/LRCollisionChannels.h"

ULRScanForInteractables::ULRScanForInteractables(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
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

	// Clean up granted abilities 
	if (AbilitySystemComponent.IsValid())
	{
		for (const auto& Pair : GrantedAbilityCache)
		{
			AbilitySystemComponent->ClearAbility(Pair.Value);
		}
	}
	GrantedAbilityCache.Empty();
	
	Super::OnDestroy(AbilityEnded);
}

void ULRScanForInteractables::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bShowDebugInfo)
	{
		DrawDebugVisualization();
	}
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

		TArray<TScriptInterface<ILRInteractableTarget>> AllTargets;
		GatherProximityTargets(AllTargets);
		FilterTargetsByMethod(AllTargets);
		break;
	}
	
}

void ULRScanForInteractables::GatherProximityTargets(TArray<TScriptInterface<ILRInteractableTarget>>& OutTargets)
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

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* OverlapActor = Result.GetActor();
		if (!OverlapActor) continue;
		
		// Check line of sight if required
		if (bRequireLineOfSight && !CheckLineOfSight(OverlapActor)) continue;

		ULRInteractionStatics::AppendInteractableTargetsFromOverlapResults(
			TArray<FOverlapResult>{Result}, OutTargets);
	}
}

void ULRScanForInteractables::FilterTargetsByMethod(TArray<TScriptInterface<ILRInteractableTarget>>& Targets)
{
	TArray<TScriptInterface<ILRInteractableTarget>> FilteredTargets;
	
	for (const auto& Target : Targets)
	{
		AActor* TargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(Target);
		if (!TargetActor) continue;
		
		// Get the interaction options to check method requirements
		FLRInteractionQuery Query;
		AActor* Avatar = GetAvatarActor();
		if (Avatar)
		{
			Query.RequestingAvatar = Avatar;
			Query.RequestingController = Cast<AController>(Avatar->GetOwner());
		}
		
		TArray<FLRInteractionOption> Options;
		FLRInteractionOptionBuilder Builder(Target, Options, Query);
		Target->GatherInteractionOptions(Query, Builder);
		
		bool bShouldInclude = false;
		
		for (const FLRInteractionOption& Option : Options)
		{
			// Check if this option's method matches what we're looking for
			switch (Option.InteractionMethod)
			{
			case EInteractionMethod::Proximity:
				// Always include proximity-based interactions when in range
				bShouldInclude = true;
				break;
				
			case EInteractionMethod::LineTrace:
				// Only include if we're looking at it
				if (IsLookingAtTarget(TargetActor))
				{
					bShouldInclude = true;
				}
				break;
				
			case EInteractionMethod::Combined:
				// Include if in cone for combined mode
				if (IsInCone(TargetActor))
				{
					bShouldInclude = true;
				}
				break;
				
			default:
				break;
			}
			
			if (bShouldInclude) break;
		}
		
		if (bShouldInclude)
		{
			FilteredTargets.Add(Target);
		}
	}
	
	Targets = FilteredTargets;

	ProcessInteractableTargets(Targets);
}

void ULRScanForInteractables::FilterTargetsByCone(TArray<TScriptInterface<ILRInteractableTarget>>& Targets)
{
	TArray<TScriptInterface<ILRInteractableTarget>> FilteredTargets;
	
	for (const auto& Target : Targets)
	{
		AActor* TargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(Target);
		if (TargetActor && IsInCone(TargetActor))
		{
			FilteredTargets.Add(Target);
		}
	}
	
	Targets = FilteredTargets;
}

void ULRScanForInteractables::ScanProximity()
{
	TArray<TScriptInterface<ILRInteractableTarget>> InteractableTargets;
	GatherProximityTargets(InteractableTargets);
	
	// For proximity-only mode, we still filter by cone if specified
	if (bScanInCone)
	{
		FilterTargetsByCone(InteractableTargets);
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

	// Calculate trace end based on view direction (includes pitch)
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
	else
	{
		// No hit, clear options
		UpdateCachedOptions(TArray<FLRInteractionOption>());
	}
}

void ULRScanForInteractables::ScanMouseOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Mouse over scan not implemented"));
}

bool ULRScanForInteractables::CheckLineOfSight(const AActor* Target) const
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

bool ULRScanForInteractables::IsInCone(const AActor* Target) const
{
	if (!Target || !bScanInCone) return true;
	
	// Use camera direction for cone check
	if (Ability && Ability->GetCurrentActorInfo())
	{
		APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
		if (PC)
		{
			FVector ViewStart;
			FRotator ViewRot;
			PC->GetPlayerViewPoint(ViewStart, ViewRot);
			
			FVector ViewDir = ViewRot.Vector();
			FVector ToTarget = Target->GetActorLocation() - ViewStart;
			ToTarget.Normalize();
			
			float DotProduct = FVector::DotProduct(ViewDir, ToTarget);
			float AngleRadians = FMath::Acos(DotProduct);
			float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
			
			return AngleDegrees <= (ScanAngle * 0.5f);
		}
	}
	
	return false;
}

bool ULRScanForInteractables::IsLookingAtTarget(const AActor* Target) const
{
	if (!Target) return false;
	
	if (Ability && Ability->GetCurrentActorInfo())
	{
		APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
		if (PC)
		{
			FVector ViewStart, TraceEnd;
			FRotator ViewRot;
			PC->GetPlayerViewPoint(ViewStart, ViewRot);
			
			FVector ViewDir = ViewRot.Vector();
			TraceEnd = ViewStart + (ViewDir * ScanRange);
			
			FCollisionQueryParams Params(SCENE_QUERY_STAT(IsLookingAt), false);
			Params.AddIgnoredActor(GetAvatarActor());
			
			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(
				HitResult,
				ViewStart,
				TraceEnd,
				LR_TraceChannel_Interaction,
				Params
			);
			
			return HitResult.bBlockingHit && HitResult.GetActor() == Target;
		}
	}
	
	return false;
}


void ULRScanForInteractables::ProcessInteractableTargets(const TArray<TScriptInterface<ILRInteractableTarget>>& Targets)
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	FLRInteractionQuery Query;
	Query.RequestingAvatar = Avatar;
	Query.RequestingController = Cast<AController>(Avatar->GetOwner());
	
	if (!AbilitySystemComponent.IsValid())
	{
		if (Ability)
		{
			AbilitySystemComponent = Ability->GetAbilitySystemComponentFromActorInfo();
		}
	}

	TArray<FLRInteractionOption> AllOptions;
	TSet<FObjectKey> CurrentFrameAbilities;

	for (const auto& Target : Targets)
	{
		TArray<FLRInteractionOption> BaseOptions;
		FLRInteractionOptionBuilder Builder(Target, BaseOptions, Query);
		Target->GatherInteractionOptions(Query, Builder);

		for (const FLRInteractionOption& BaseOption : BaseOptions)
		{
			FLRInteractionOption EnhancedOption = BaseOption;
			
			AActor* TargetActor = ULRInteractionStatics::GetActorFromInteractableTarget(Target);
			
			bool bShouldGrantAbility = false;
			
			switch (EnhancedOption.InteractionMethod)
			{
			case EInteractionMethod::Proximity:
				bShouldGrantAbility = true; 
				break;
				
			case EInteractionMethod::LineTrace:
				bShouldGrantAbility = IsLookingAtTarget(TargetActor);
				break;
				
			case EInteractionMethod::Combined:
			case EInteractionMethod::MouseOver:
				bShouldGrantAbility = IsInCone(TargetActor);
				break;
			}
			
			if (bShouldGrantAbility && EnhancedOption.Requirements.CheckRequirements(Avatar, TargetActor))
			{
				if (EnhancedOption.InteractionAbilityToGrant && AbilitySystemComponent.IsValid())
				{
					FObjectKey ObjectKey(EnhancedOption.InteractionAbilityToGrant);
					CurrentFrameAbilities.Add(ObjectKey);
					
					if (!GrantedAbilityCache.Contains(ObjectKey))
					{
						FGameplayAbilitySpec Spec(EnhancedOption.InteractionAbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
						GrantedAbilityCache.Add(ObjectKey, Handle);
					}
				}
				
				if (IsInCone(TargetActor))
				{
					AllOptions.Add(EnhancedOption);
				}
			}
		}
	}
	
	TArray<FObjectKey> KeysToRemove;
	for (const auto& Pair : GrantedAbilityCache)
	{
		if (!CurrentFrameAbilities.Contains(Pair.Key))
		{
			if (AbilitySystemComponent.IsValid())
			{
				AbilitySystemComponent->ClearAbility(Pair.Value);
			}
			KeysToRemove.Add(Pair.Key);
		}
	}
	
	for (const FObjectKey& Key : KeysToRemove)
	{
		GrantedAbilityCache.Remove(Key);
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
			if (A.Priority != B.Priority)
			{
				return A.Priority > B.Priority;
			}

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

void ULRScanForInteractables::DrawDebugVisualization()
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	if (!World) return;

	FVector CharLocation = Avatar->GetActorLocation();
	DrawDebugSphere(World, CharLocation, ScanRange, 32, FColor::Yellow, false, -1.0f, 0, 1.0f);

	if (Ability && Ability->GetCurrentActorInfo())
	{
		APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
		if (PC)
		{
			// Get camera orientation
			FVector CamLoc;
			FRotator CamRot;
			PC->GetPlayerViewPoint(CamLoc, CamRot);

			FVector ViewDir = CamRot.Vector();
			FVector TraceEnd = CharLocation + (ViewDir * ScanRange);

			// Main trace line: from character toward where camera points
			DrawDebugLine(World, CharLocation, TraceEnd, FColor::Red, false, -1.0f, 0, 2.0f);

			if (bScanInCone && ScanAngle < 180.0f)
			{
				float HalfAngle = ScanAngle * 0.5f;

				FRotator LeftRot = CamRot;
				LeftRot.Yaw -= HalfAngle;
				FVector LeftDir = LeftRot.Vector();

				FRotator RightRot = CamRot;
				RightRot.Yaw += HalfAngle;
				FVector RightDir = RightRot.Vector();

				FRotator TopRot = CamRot;
				TopRot.Pitch += HalfAngle;
				FVector TopDir = TopRot.Vector();

				FRotator BottomRot = CamRot;
				BottomRot.Pitch -= HalfAngle;
				FVector BottomDir = BottomRot.Vector();

				// Draw cone edges (all starting from character, not camera)
				DrawDebugLine(World, CharLocation, CharLocation + LeftDir * ScanRange, FColor::Green, false, -1.0f, 0, 1.0f);
				DrawDebugLine(World, CharLocation, CharLocation + RightDir * ScanRange, FColor::Green, false, -1.0f, 0, 1.0f);
				DrawDebugLine(World, CharLocation, CharLocation + TopDir * ScanRange, FColor::Blue, false, -1.0f, 0, 1.0f);
				DrawDebugLine(World, CharLocation, CharLocation + BottomDir * ScanRange, FColor::Blue, false, -1.0f, 0, 1.0f);

				// Circle for cone base
				FVector Normal = ViewDir.GetSafeNormal();
				FVector Right = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Y);
				FVector Up = FVector::CrossProduct(Normal, Right).GetSafeNormal();

				DrawDebugCircle(
					World,
					CharLocation + Normal * ScanRange, // from character, not camera
					ScanRange * FMath::Tan(FMath::DegreesToRadians(HalfAngle)),
					32,
					FColor::Cyan,
					false,
					-1.0f,
					0,
					1.0f,
					Right,
					Up,
					false
				);
			}
		}
	}
}
