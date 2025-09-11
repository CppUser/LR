// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/LRInteractionTypes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool FInteractionRequirement::CheckRequirements(const AActor* Interactor, const AActor* Target) const
{
	if (!Interactor || !Target) return false;

    // Check distance
    float Distance = FVector::Dist(Interactor->GetActorLocation(), Target->GetActorLocation());
    if (Distance < MinDistance || Distance > MaxDistance)
        return false;

    // Check required angle
    if (RequiredAngle >= 0)
    {
        FVector ToInteractor = Interactor->GetActorLocation() - Target->GetActorLocation();
        ToInteractor.Z = 0;
        ToInteractor.Normalize();

        FVector TargetForward = Target->GetActorForwardVector();
        TargetForward.Z = 0;
        TargetForward.Normalize();

        float DotProduct = FVector::DotProduct(TargetForward, ToInteractor);
        float AngleRadians = FMath::Acos(DotProduct);
        float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

        // Check if within required angle range (±15 degrees tolerance)
        if (FMath::Abs(AngleDegrees - RequiredAngle) > 15.0f)
            return false;
    }

    // Check tags
    if (RequiredTags.Num() > 0 || BlockedTags.Num() > 0)
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Interactor))
        {
            FGameplayTagContainer InteractorTags;
            ASC->GetOwnedGameplayTags(InteractorTags);

            if (!InteractorTags.HasAll(RequiredTags))
                return false;

            if (InteractorTags.HasAny(BlockedTags))
                return false;
        }
    }

    // Check line of sight if required
    if (bRequiresLineOfSight)
    {
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Interactor);
        Params.AddIgnoredActor(Target);

        FHitResult HitResult;
        bool bBlocked = Interactor->GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Interactor->GetActorLocation() + FVector(0, 0, 50),
            Target->GetActorLocation() + FVector(0, 0, 50),
            ECC_Visibility,
            Params
        );

        if (bBlocked)
            return false;
    }

    return true;
}
