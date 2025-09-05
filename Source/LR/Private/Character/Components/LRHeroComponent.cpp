// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Character/Components/LRHeroComponent.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/Core/LRAbilitySystemComponent.h"
#include "Character/LRPawnData.h"
#include "Character/Components/LRPawnExtComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Input/LRInputComponent.h"
#include "Player/LRLocalPlayer.h"
#include "Player/LRPlayerController.h"
#include "Player/LRPlayerState.h"
#include "Utils/LRGameplayTags.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

const FName ULRHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName ULRHeroComponent::NAME_ActorFeatureName("Hero");

ULRHeroComponent::ULRHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
}

void ULRHeroComponent::AddAdditionalInputConfig(const ULRInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const ULRPawnExtComponent* PawnExtComp = ULRPawnExtComponent::FindPawnExtComponent(Pawn))
	{
		ULRInputComponent* IC = Pawn->FindComponentByClass<ULRInputComponent>();
		if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULRInputComponent or a subclass of it.")))
		{
			IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void ULRHeroComponent::RemoveAdditionalInputConfig(const ULRInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool ULRHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

bool ULRHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == LRGameplayTags::InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == LRGameplayTags::InitState_Spawned && DesiredState == LRGameplayTags::InitState_DataAvailable)
	{
		if (!GetPlayerState<ALRPlayerState>())
		{
			return false;
		}

		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ALRPlayerController* PC = GetController<ALRPlayerController>();

			if (!Pawn->InputComponent || !PC || !PC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == LRGameplayTags::InitState_DataAvailable && DesiredState == LRGameplayTags::InitState_DataInitialized)
	{
		ALRPlayerState* PS = GetPlayerState<ALRPlayerState>();

		return PS && Manager->HasFeatureReachedInitState(Pawn, ULRPawnExtComponent::NAME_ActorFeatureName, LRGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == LRGameplayTags::InitState_DataInitialized && DesiredState == LRGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void ULRHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (CurrentState == LRGameplayTags::InitState_DataAvailable && DesiredState == LRGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALRPlayerState* PS = GetPlayerState<ALRPlayerState>();
		if (!ensure(Pawn && PS))
		{
			return;
		}

		const ULRPawnData* PawnData = nullptr;

		if (ULRPawnExtComponent* PawnExtComp = ULRPawnExtComponent::FindPawnExtComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<ULRPawnData>();

			PawnExtComp->InitializeAbilitySystem(PS->GetLRAbilitySystemComponent(), PS);
		}

		if (ALRPlayerController* PC = GetController<ALRPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// if (PawnData)
		// {
		// 	if (ULRCameraComponent* CameraComponent = ULRCameraComponent::FindCameraComponent(Pawn))
		// 	{
		//TODO: 		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		// 	}
		// }
	}
}

void ULRHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == ULRPawnExtComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == LRGameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void ULRHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { LRGameplayTags::InitState_Spawned, LRGameplayTags::InitState_DataAvailable, LRGameplayTags::InitState_DataInitialized, LRGameplayTags::InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

void ULRHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogTemp, Error, TEXT("[ULRHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("LRHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("LyraHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		RegisterInitStateFeature();
	}
}

void ULRHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(ULRPawnExtComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	ensure(TryToChangeInitState(LRGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void ULRHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void ULRHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULRLocalPlayer* LP = Cast<ULRLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const ULRPawnExtComponent* PawnExtComp = ULRPawnExtComponent::FindPawnExtComponent(Pawn))
	{
		if (const ULRPawnData* PawnData = PawnExtComp->GetPawnData<ULRPawnData>())
		{
			if (const ULRInputConfig* InputConfig = PawnData->InputConfig)
			{
				//TODO: for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				// {
				// 	if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
				// 	{
				// 		if (Mapping.bRegisterWithSettings)
				// 		{
				// 			if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
				// 			{
				// 				Settings->RegisterInputMappingContext(IMC);
				// 			}
				// 			
				// 			FModifyContextOptions Options = {};
				// 			Options.bIgnoreAllPressedKeysUntilRelease = false;
				// 			// Actually add the config to the local player							
				// 			Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
				// 		}
				// 	}
				// }

				ULRInputComponent* IC = Cast<ULRInputComponent>(PlayerInputComponent);
				if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULRInputComponent or a subclass of it.")))
				{
					IC->AddInputMappings(InputConfig, Subsystem);

					TArray<uint32> BindHandles;
					IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					IC->BindNativeAction(InputConfig, LRGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
					IC->BindNativeAction(InputConfig, LRGameplayTags::InputTag_Mouse_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void ULRHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const ULRPawnExtComponent* PawnExtComp = ULRPawnExtComponent::FindPawnExtComponent(Pawn))
		{
			if (ULRAbilitySystemComponent* ASC = PawnExtComp->GetLRAbilitySystemComponent())
			{
				ASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void ULRHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const ULRPawnExtComponent* PawnExtComp = ULRPawnExtComponent::FindPawnExtComponent(Pawn))
	{
		if (ULRAbilitySystemComponent* ASC = PawnExtComp->GetLRAbilitySystemComponent())
		{
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void ULRHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void ULRHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}
