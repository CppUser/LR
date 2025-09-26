// Fill out your copyright notice in the Description page of Project Settings.


#include "Experemental/HTN/Test/AITest.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Experemental/HTN/HTNWorldState.h"
#include "Navigation/PathFollowingComponent.h"

float UAITask_MoveToLocation::CalculateUtility_Implementation(UHTNWorldState* WorldState) const
{
	if (!WorldState || !AIController) return 0.0f;
    
	float BaseUtility = Super::CalculateUtility_Implementation(WorldState);
    
	APawn* Pawn = AIController->GetPawn();
	if (Pawn)
	{
		float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetLocation);
        
		float DistanceUtility = FMath::Clamp(1.0f - (Distance / 10000.0f), 0.0f, 1.0f);
		BaseUtility *= DistanceUtility;
	}
    
	return BaseUtility;
}

void UAITask_MoveToLocation::OnExecuteAction_Implementation(UHTNWorldState* WorldState)
{
	if (!AIController) return;

	//TODO: Fix implementation
	
	// UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	// if (!NavSystem) 
	// {
	// 	SetStatus(EHTNTaskStatus::Failed);
	// 	return;
	// }
 //    
	// FAIMoveRequest MoveRequest(TargetLocation);
	// MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	// MoveRequest.SetUsePathfinding(true);
 //    
	// FPathFollowingRequestResult Result = AIController->MoveTo(MoveRequest);
 //    
	// if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
	// {
	// 	bIsMoving = true;
	// 	UE_LOG(LogHTN, Log, TEXT("Moving to location: %s"), *TargetLocation.ToString());
	// }
	// else
	// {
	// 	UE_LOG(LogHTN, Warning, TEXT("Failed to start movement to: %s"), *TargetLocation.ToString());
	// 	SetStatus(EHTNTaskStatus::Failed);
	// }
}

void UAITask_MoveToLocation::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (!bIsMoving || !AIController) return;
    
	if (AIController->GetPathFollowingComponent())
	{
		EPathFollowingStatus::Type MoveStatus = AIController->GetPathFollowingComponent()->GetStatus();
        
		if (MoveStatus == EPathFollowingStatus::Idle)
		{
			APawn* ControlledPawn = AIController->GetPawn();
			if (ControlledPawn)
			{
				float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), TargetLocation);
                
				if (Distance <= AcceptanceRadius)
				{
					SetStatus(EHTNTaskStatus::Success);
					bIsMoving = false;
					UE_LOG(LogHTN, Log, TEXT("Reached destination"));
				}
				else
				{
					SetStatus(EHTNTaskStatus::Failed);
					bIsMoving = false;
					UE_LOG(LogHTN, Warning, TEXT("Movement stopped before reaching destination"));
				}
			}
		}
	}
}

////////////////////////////////////////////////
///
////////////////////////////////////////////////
UAITask_Combat::UAITask_Combat()
{
	TaskName = TEXT("Combat");
	Priority = 10;
	UtilityScore = 5.0f;
    
	SetupCombatMethods();
}

FHTNMethod UAITask_Combat::SelectBestMethod_Implementation(const TArray<FHTNMethod>& ValidMethods,
	UHTNWorldState* WorldState) const
{
	if (ValidMethods.Num() == 0) return FHTNMethod();
    
    FHTNMethod BestMethod = ValidMethods[0];
    float BestScore = 0.0f;
    
    for (const FHTNMethod& Method : ValidMethods)
    {
        float Score = Method.PreferenceScore;
        
        if (Method.MethodName == TEXT("RangedCombat"))
        {
            FHTNWorldProperty HealthProperty = WorldState->GetProperty(TEXT("Health"));
            if (HealthProperty.NumericValue < 50.0f)
            {
                Score *= 2.0f;
            }
            
            FHTNWorldProperty AmmoProperty = WorldState->GetProperty(TEXT("AmmoCount"));
            Score *= FMath::Min(1.0f, AmmoProperty.NumericValue / 10.0f);
        }
        else if (Method.MethodName == TEXT("MeleeCombat"))
        {
            FHTNWorldProperty StrengthProperty = WorldState->GetProperty(TEXT("Strength"));
            Score *= (StrengthProperty.NumericValue / 100.0f);
            
            FHTNWorldProperty ArmorProperty = WorldState->GetProperty(TEXT("Armor"));
            Score *= (1.0f + ArmorProperty.NumericValue / 100.0f);
        }
        else if (Method.MethodName == TEXT("StealthCombat"))
        {
            Score *= 2.5f;
            
            FHTNWorldProperty LightingProperty = WorldState->GetProperty(TEXT("AmbientLighting"));
            if (LightingProperty.NumericValue < 0.3f) // Dark environment
            {
                Score *= 1.5f;
            }
        }
        
        if (Target)
        {
            FHTNWorldProperty TargetHealthProperty = WorldState->GetProperty(TEXT("TargetHealth"));
            FHTNWorldProperty TargetArmorProperty = WorldState->GetProperty(TEXT("TargetArmor"));
            
            float TargetDifficulty = (TargetHealthProperty.NumericValue + TargetArmorProperty.NumericValue) / 200.0f;
            
            if (Method.MethodName == TEXT("StealthCombat") && TargetDifficulty > 0.7f)
            {
                Score *= 1.5f; 
            }
        }
        
        if (Score > BestScore)
        {
            BestScore = Score;
            BestMethod = Method;
        }
    }
    
    UE_LOG(LogHTN, Log, TEXT("Selected combat method: %s (Score: %.2f)"), 
           *BestMethod.MethodName.ToString(), BestScore);
    
    return BestMethod;
}

void UAITask_Combat::SetupCombatMethods()
{
	// Ranged combat method
	FHTNMethod RangedMethod;
	RangedMethod.MethodName = TEXT("RangedCombat");
	RangedMethod.PreferenceScore = 2.0f;
    
	// Preconditions for ranged combat
	FHTNCondition HasAmmo;
	HasAmmo.PropertyName = TEXT("AmmoCount");
	HasAmmo.CheckNumeric = true;
	HasAmmo.NumericThreshold = 1.0f;
	RangedMethod.Preconditions.Add(HasAmmo);
    
	FHTNCondition HasWeapon;
	HasWeapon.PropertyName = TEXT("HasRangedWeapon");
	//TODO:HasWeapon.BoolValue = true;
	RangedMethod.Preconditions.Add(HasWeapon);
    
	Methods.Add(RangedMethod);
    
	// Melee combat method
	FHTNMethod MeleeMethod;
	MeleeMethod.MethodName = TEXT("MeleeCombat");
	MeleeMethod.PreferenceScore = 1.0f;
    
	FHTNCondition InMeleeRange;
	InMeleeRange.PropertyName = TEXT("DistanceToTarget");
	InMeleeRange.CheckNumeric = true;
	InMeleeRange.NumericThreshold = 200.0f;
	MeleeMethod.Preconditions.Add(InMeleeRange);
    
	FHTNCondition HasMeleeWeapon;
	HasMeleeWeapon.PropertyName = TEXT("HasMeleeWeapon");
	//TODO:HasMeleeWeapon.BoolValue = true;
	MeleeMethod.Preconditions.Add(HasMeleeWeapon);
    
	Methods.Add(MeleeMethod);
    
	// Stealth combat method
	FHTNMethod StealthMethod;
	StealthMethod.MethodName = TEXT("StealthCombat");
	StealthMethod.PreferenceScore = 3.0f; // Preferred if available
    
	FHTNCondition NotDetected;
	NotDetected.PropertyName = TEXT("IsDetected");
	//TODO:NotDetected.BoolValue = false;
	StealthMethod.Preconditions.Add(NotDetected);
    
	FHTNCondition CanStealth;
	CanStealth.PropertyName = TEXT("StealthSkill");
	CanStealth.CheckNumeric = true;
	CanStealth.NumericThreshold = 50.0f;
	StealthMethod.Preconditions.Add(CanStealth);
    
	Methods.Add(StealthMethod);
}
