// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "ConfigurationStyle.h"

class FConfigurationCommands : public TCommands<FConfigurationCommands>
{
public:

	FConfigurationCommands()
		: TCommands<FConfigurationCommands>(TEXT("Configuration"), NSLOCTEXT("Contexts", "Configuration", "Configuration Plugin"), NAME_None, FConfigurationStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};