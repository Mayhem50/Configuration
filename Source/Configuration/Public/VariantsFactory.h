#pragma once

#include "UnrealEd.h"
#include "VariantsFactory.generated.h"



UCLASS()
class UVariantsFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

		// UFactory interface
		virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};