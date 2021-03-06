// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ConfigurationPrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "ConfigurationStyle.h"
#include "ConfigurationCommands.h"

#include "LevelEditor.h"

#include "LayerWidget.h"
#include "VariantWidget.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"

#include "AssetRegistryModule.h"

static const FName ConfigurationTabName("Configuration");

#define LOCTEXT_NAMESPACE "FConfigurationModule"

void FConfigurationModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FPackageName::RegisterMountPoint("/ConfigurationModule/", FPaths::GamePluginsDir());

	FConfigurationStyle::Initialize();
	FConfigurationStyle::ReloadTextures();

	FConfigurationCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FConfigurationCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FConfigurationModule::PluginButtonClicked),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FConfigurationModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FConfigurationModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ConfigurationTabName, FOnSpawnTab::CreateRaw(this, &FConfigurationModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FConfigurationTabTitle", "Configuration"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	OnInit();
}

void FConfigurationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FConfigurationStyle::Shutdown();

	FConfigurationCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ConfigurationTabName);
}

void FConfigurationModule::OnInit()
{
	InitLayerManager();
	InitVariantManager();

	FEditorDelegates::OnApplyObjectToActor.AddUObject(LayerManager, &ULayerManager::OnApplyObjectOnActor);
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(LayerManager, &ULayerManager::OnObjectPropertyChanged);
}

void FConfigurationModule::InitVariantManager()
{
	/*UPackage* Package = LoadPackage(nullptr, TEXT("/ConfigurationModule/Datas/Variants"), LOAD_None);

	if(!Package)
	{
		Package = CreatePackage(nullptr, TEXT("/ConfigurationModule/Datas/Variants"));

		if(!Package)
		{
			return;
		}

		VariantManager = NewObject<UVariantManager>(Package, UVariantManager::StaticClass(), TEXT("Variants"), RF_Public | RF_Standalone);
		FAssetRegistryModule::AssetCreated(VariantManager);
		VariantManager->MarkPackageDirty();
	}
	else
	{
		VariantManager = FindObject<UVariantManager>(Package, TEXT("Layers"));
	}

	VariantManager->AddToRoot();*/
}

void FConfigurationModule::InitLayerManager()
{
	UPackage* Package = LoadPackage(nullptr, TEXT("/ConfigurationModule/Datas/Layers"), LOAD_None);

	if(!Package)
	{
		Package = CreatePackage(nullptr, TEXT("/ConfigurationModule/Datas/Layers"));

		if(!Package)
		{
			return;
		}

		LayerManager = NewObject<ULayerManager>(Package, ULayerManager::StaticClass(), TEXT("Layers"), RF_Public | RF_Standalone);
		FAssetRegistryModule::AssetCreated(LayerManager);
		LayerManager->MarkPackageDirty();
	}
	else
	{
		LayerManager = FindObject<ULayerManager>(Package, TEXT("Layers"));
	}

	LayerManager->AddToRoot();
}

TSharedRef<SDockTab> FConfigurationModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	LayerManager->Init();

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SLayersWidget).LayerManager(LayerManager)
			]

			+ SHorizontalBox::Slot()
			[
				SNew(SVariantsWidget).VariantManager(VariantManager)
			]
		];
}

void FConfigurationModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(ConfigurationTabName);
}

void FConfigurationModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FConfigurationCommands::Get().OpenPluginWindow);
}

void FConfigurationModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FConfigurationCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FConfigurationModule, Configuration)
