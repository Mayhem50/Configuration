#include "ConfigurationPrivatePCH.h"

#include "Value.h"

#define LOCTEXT_NAMESPACE "FConfigurationModule"


UConfigurationValue::UConfigurationValue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UConfigurationValue::Apply()
{
	for(UBaseLayer* Layer : Layers)
	{
		Layer->Apply();
	}
}

#undef LOCTEXT_NAMESPACE
