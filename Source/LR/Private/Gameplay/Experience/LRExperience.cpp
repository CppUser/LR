// Fill out your copyright notice in the Description page of Project Settings.


#include "LR/Public/Gameplay/Experience/LRExperience.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "LRSystem"

#if WITH_EDITOR
EDataValidationResult ULRExperience::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);


	// Make sure users didn't subclass from a BP of this (it's fine and expected to subclass once in BP, just not twice)
	if (!GetClass()->IsNative())
	{
		const UClass* ParentClass = GetClass()->GetSuperClass();

		// Find the native parent
		const UClass* FirstNativeParent = ParentClass;
		while ((FirstNativeParent != nullptr) && !FirstNativeParent->IsNative())
		{
			FirstNativeParent = FirstNativeParent->GetSuperClass();
		}

		if (FirstNativeParent != ParentClass)
		{
			Context.AddError(FText::Format(LOCTEXT("ExperienceInheritenceIsUnsupported", "Blueprint subclasses of Blueprint experiences is not currently supported (use composition via ActionSets instead). Parent class was {0} but should be {1}."), 
				FText::AsCultureInvariant(GetPathNameSafe(ParentClass)),
				FText::AsCultureInvariant(GetPathNameSafe(FirstNativeParent))
			));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif
#if WITH_EDITORONLY_DATA
void ULRExperience::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();
	
}
#endif
#undef LOCTEXT_NAMESPACE