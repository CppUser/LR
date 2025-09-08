// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_HTN.h"

#include "HTN.h"
#include "HTNEditorModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"
FText AssetTypeActions_HTN::GetName() const
{
	return LOCTEXT("HTNAssetName", "Hierarchical Task Network");
}

FColor AssetTypeActions_HTN::GetTypeColor() const
{
	return FColor(182, 61, 14);
}

UClass* AssetTypeActions_HTN::GetSupportedClass() const
{
	return UHTN::StaticClass();
}

uint32 AssetTypeActions_HTN::GetCategories()
{
	return IAssetTools::Get().FindAdvancedAssetCategory("Artificial Intelligence");
}

void AssetTypeActions_HTN::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* const Object : InObjects)
	{
		if (UHTN* const HTN = Cast<UHTN>(Object))
		{
			FHTNEditorModule::Get().CreateEditor(Mode, EditWithinLevelEditor, HTN);
		}
	}
}

void AssetTypeActions_HTN::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	TArray<TWeakObjectPtr<UHTN>> HTNs;
	for (UObject* Object : InObjects)
	{
		if (UHTN* HTN = Cast<UHTN>(Object))
		{
			HTNs.Add(HTN);
		}
	}
    
	if (HTNs.Num() > 0)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ValidateHTN", "Validate HTN"),
			LOCTEXT("ValidateHTNTooltip", "Check HTN for errors"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([HTNs]()
			{
				for (TWeakObjectPtr<UHTN> HTN : HTNs)
				{
					if (HTN.IsValid())
					{
						// Validate HTN
						TArray<FString> Errors;
						if (HTN->ValidateAsset(Errors))
						{
							UE_LOG(LogTemp, Log, TEXT("HTN %s is valid"), 
								*HTN->GetName());
						}
						else
						{
							for (const FString& Error : Errors)
							{
								UE_LOG(LogTemp, Error, TEXT("HTN %s: %s"), 
									*HTN->GetName(), *Error);
							}
						}
					}
				}
			}))
		);
	}
}
#undef LOCTEXT_NAMESPACE