#pragma once

#include "Classes/Engine/DataAsset.h"

#include "Parameter.generated.h"

UCLASS(Blueprintable)
class CONFIGURATION_API UConfigurationParameter : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Apply();

	UFUNCTION(BlueprintCallable, Category = "Functions")
		void SetEnable(bool bInEnable);

	UPROPERTY(EditAnywhere, Category = "Properties")
		FString Name;

	UPROPERTY(EditAnywhere, Category = "Properties")
		bool bEnable;

	UPROPERTY(EditAnywhere, Category = "Properties")
		TArray<class UConfigurationValue*> Values;

	UPROPERTY(EditAnywhere, Category = "Properties")
		class UConfigurationValue* CurrentValue;
};
