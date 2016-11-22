// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseLayer.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ELayerEnum : uint8
{
	MATERIAL 	UMETA(DisplayName = "Material"),
	POSITION 	UMETA(DisplayName = "Position"),
	GEOMETRY	UMETA(DisplayName = "Geometry")
};

UCLASS()
class CONFIGURATION_API UBaseLayer : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UBaseLayer(const FObjectInitializer& ObjectInitializer);
	virtual ~UBaseLayer() {}

	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Init(ELayerEnum Type, const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Functions")
		virtual void Apply();

	UFUNCTION(BlueprintCallable, Category = "Functions")
		bool  IsEnabled();

	UFUNCTION(BlueprintCallable, Category = "Functions")
		void  SetEnabled(bool InEnable);

	DECLARE_DELEGATE(FOnEnabledChanged)
	FOnEnabledChanged OnEnabledChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Properties")
		FString LayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Properties")
		ELayerEnum Type;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Properties")
		bool bEnabled;
};
