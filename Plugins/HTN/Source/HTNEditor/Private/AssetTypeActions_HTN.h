#pragma once

#include "AssetTypeActions_Base.h"

class AssetTypeActions_HTN : public FAssetTypeActions_Base
{
public:
	AssetTypeActions_HTN(EAssetTypeCategories::Type InAssetCategory);

	
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;

private:
	EAssetTypeCategories::Type AssetCategory;
};
