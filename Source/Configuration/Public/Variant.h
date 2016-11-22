#pragma once

#include "Classes/Engine/DataAsset.h"

#include "Variant.generated.h"

UCLASS(Blueprintable)
class UConfigurationVariant : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Properties")
		FString Name;

	UPROPERTY(EditAnywhere, Category = "Properties")
		TArray<class UConfigurationParameter*> Parameters;
};

