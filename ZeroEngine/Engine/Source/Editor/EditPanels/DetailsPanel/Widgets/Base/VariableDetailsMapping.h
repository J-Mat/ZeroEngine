#pragma once
#include "DetailsMapping.h"
#include "World/Base/VariateProperty.h"

namespace Zero
{
	class FVariableDetailsMapping : public FDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty) { return false; }
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return nullptr; }
	};
}