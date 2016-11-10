// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ConfigurationPrivatePCH.h"
#include "ConfigurationCommands.h"

#define LOCTEXT_NAMESPACE "FConfigurationModule"

void FConfigurationCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Configuration", "Bring up Configuration window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
