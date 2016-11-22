#pragma once

#include "Classes/Engine/DataAsset.h"

#include "Parameter.generated.h"

UCLASS(Blueprintable)
class CONFIGURATION_API UConfigurationParameter : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Properties")
		FString Name;

	UPROPERTY(EditAnywhere, Category = "Properties")
		TArray<class UConfigurationValue*> Values;
};
