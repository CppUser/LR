
#pragma once
#include "NativeGameplayTags.h"

namespace LRGameplayTags
{
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
	
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
	
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Mouse_Look);
	
	LR_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
}
