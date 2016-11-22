#pragma once

#include "Classes/Engine/DataAsset.h"

#include "Value.generated.h"

UCLASS(Blueprintable)
class CONFIGURATION_API UConfigurationValue : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Properties")
		FString Name;

	UPROPERTY(EditAnywhere, Category = "Properties")
		TArray<class UBaseLayer*> Layers;
};
