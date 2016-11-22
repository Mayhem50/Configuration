#pragma once

#include "AssetTypeActions_Base.h"
#include "Variant.h"

class FAssetTypeActions_ConfigurationVariant : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_ConfigurationVariant", "Configuration Variants"); }
	virtual FColor GetTypeColor() const override { return FColor(175, 0, 128); }
	virtual UClass* GetSupportedClass() const override { return UConfigurationVariant::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
};
