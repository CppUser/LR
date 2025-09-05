// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Gameplay/GameMode/LRGameMode.h"
#include "Character/LRPawnData.h"
#include "Character/LRCharacter.h"
#include "Character/Components/LRPawnExtComponent.h"
#include "Gameplay/Experience/LRExperience.h"
#include "Gameplay/Experience/LRExperienceManagerComponent.h"
#include "Gameplay/GameMode/LRGameState.h"
#include "Gameplay/GameMode/LRWorldSettings.h"
#include "Player/LRPlayerController.h"
#include "Player/LRPlayerState.h"
#include "System/LRAssetManager.h"
#include "UI/LRHUD.h"

ALRGameMode::ALRGameMode(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	DefaultPawnClass = ALRCharacter::StaticClass();
	PlayerControllerClass = ALRPlayerController::StaticClass();
	PlayerStateClass = ALRPlayerState::StaticClass();
	GameStateClass = ALRGameState::StaticClass();
	HUDClass = ALRHUD::StaticClass();
}

const ULRPawnData* ALRGameMode::GetPawnDataForController(const AController* InController) const
{
	if (InController != nullptr)
	{
		if (const ALRPlayerState* PS = InController->GetPlayerState<ALRPlayerState>())
		{
			if (const ULRPawnData* PawnData = PS->GetPawnData<ULRPawnData>())
			{
				return PawnData;
			}
		}
	}

	check(GameState);
	ULRExperienceManagerComponent* ExperienceManager = GameState->FindComponentByClass<ULRExperienceManagerComponent>();
	check(ExperienceManager);

	if (ExperienceManager->IsExperienceLoaded())
	{
		const ULRExperience* Experience = ExperienceManager->GetCurrentExperienceChecked();
		if (Experience->PawnData !=nullptr)
		{
			return Experience->PawnData;
		}

		//TODO: get from asset manager
		//return ULRAssetManager::Get().GetDefaultPawnData();
		return nullptr;
	}
	return nullptr;
}

void ALRGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this,&ThisClass::PrepForExperienceLoading);
}

void ALRGameMode::OnExperienceLoaded(const ULRExperience* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

UClass* ALRGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const ULRPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ALRGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (ULRPawnExtComponent* PawnExtComp = ULRPawnExtComponent::FindPawnExtComponent(SpawnedPawn))
			{
				if (const ULRPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool ALRGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

void ALRGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* ALRGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//TODO: implement UASPlayerSpawningManagerComponent
	// if (UASPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UASPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ChoosePlayerStart(Player);
	// }
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ALRGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	//TODO: implement UASPlayerSpawningManagerComponent
	// if (UASPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UASPlayerSpawningManagerComponent>())
	// {
	// PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	// }
	
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool ALRGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

void ALRGameMode::InitGameState()
{
	Super::InitGameState();

	ULRExperienceManagerComponent* ExperienceManager = GameState->FindComponentByClass<ULRExperienceManagerComponent>();
	check(ExperienceManager);
	ExperienceManager->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this,&ThisClass::OnExperienceLoaded));
}

bool ALRGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

void ALRGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void ALRGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(LogTemp, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}

bool ALRGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	//TODO: implement UASPlayerSpawningManagerComponent
	// if (UASPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UASPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ControllerCanRestart(Controller);
	// }

	return true;
}

void ALRGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		//TODO: That online way to request , find offline way
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	//TODO: implement AASAIController
	// else if (AASAIController* BotController = Cast<AASAIController>(Controller))
	// {
	// 	GetWorldTimerManager().SetTimerForNextTick(BotController, &AASAIController::ServerRestartController);
	// }
}

bool ALRGameMode::IsExperienceLoaded() const
{
	check(GameState);
	ULRExperienceManagerComponent* ExperienceManager = GameState->FindComponentByClass<ULRExperienceManagerComponent>();
	check(ExperienceManager);
	return ExperienceManager->IsExperienceLoaded();
}

void ALRGameMode::PrepForExperienceLoading()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceSource;

	//TODO: Currently loading from world settings default experience. impl more options to search experience

	if (!ExperienceId.IsValid())
	{
		if (ALRWorldSettings* WS = Cast<ALRWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = WS->GetDefaultGameplayExperience();
			ExperienceSource = TEXT("WorldSettings");
		}
	}

	ULRAssetManager& AssetManager = ULRAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId,Dummy))
	{
		UE_LOG(LogTemp,Error,TEXT("EXPERIENCE:Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	OnExperienceDataReady(ExperienceId,ExperienceSource);
}

void ALRGameMode::OnExperienceDataReady(FPrimaryAssetId ExperienceId, const FString& ExperienceSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogTemp,Log,TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(),*ExperienceSource);

		ULRExperienceManagerComponent* ExperienceManager = GameState->FindComponentByClass<ULRExperienceManagerComponent>();
		check(ExperienceManager);
		ExperienceManager->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}


