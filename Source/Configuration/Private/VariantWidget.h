#pragma once

#include "SlateBasics.h"
#include "SBorder.h"

class SVariantsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVariantsWidget) {}

	SLATE_ARGUMENT(class UVariantManager*, VariantManager)

		SLATE_END_ARGS()

		void Construct(const FArguments& Args);

private:
	class UVariantManager* VariantManager;
};
