// Fill out your copyright notice in the Description page of Project Settings.

#include "ConfigurationPrivatePCH.h"

#include "Configuration.h"
#include "BaseLayer.h"

UBaseLayer::UBaseLayer(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer){}

// Sets default values
void UBaseLayer::Init(ELayerEnum InType, const FString& InName)
{
	Type = InType;
	LayerName = InName;
	bEnabled = true;
}

void UBaseLayer::Apply()
{

}

bool UBaseLayer::IsEnabled ()
{
	return bEnabled;
}

void UBaseLayer::SetEnabled (bool InEnabled)
{
	bEnabled = InEnabled;
	OnEnabledChanged.ExecuteIfBound ();
}

