#pragma once

#include "Classes/Engine/DataAsset.h"

#include "VariantManager.generated.h"

UCLASS(Blueprintable)
class UVariantManager : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
		TArray<class UConfigurationParameter*> Parameters;
};

