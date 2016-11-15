// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULayerInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class CONFIGURATION_API ILayerInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION (BlueprintNativeEvent, BlueprintCallable, Category = "Functions")
		void Apply ();
};
