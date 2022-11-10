#pragma once
#include "DetailsMapping.h"
#include "World/Base/VariateProperty.h"

namespace Zero
{
	class FVariableDetailsMapping : public FDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return nullptr; }
	};
}