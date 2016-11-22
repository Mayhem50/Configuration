#include "ConfigurationPrivatePCH.h"

#include "VariantsFactory.h"
#include "Variant.h"

#define LOCTEXT_NAMESPACE "FConfigurationModule"

UVariantsFactory::UVariantsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UConfigurationVariant::StaticClass();
}

UObject* UVariantsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UConfigurationVariant* NewObjectAsset = NewObject<UConfigurationVariant>(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

#undef LOCTEXT_NAMESPACE