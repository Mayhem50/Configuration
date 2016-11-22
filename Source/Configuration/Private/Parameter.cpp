#include "ConfigurationPrivatePCH.h"

#include "Parameter.h"
#include "Value.h"

#define LOCTEXT_NAMESPACE "FConfigurationModule"

UConfigurationParameter::UConfigurationParameter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bEnable = true;
}

void UConfigurationParameter::Apply()
{
	if(CurrentValue && bEnable)
	{
		CurrentValue->Apply();
	}
}

void UConfigurationParameter::SetEnable(bool bInEnable)
{
	bEnable = bInEnable;

	Apply();
}

#undef LOCTEXT_NAMESPACE
