#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FFloatDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FFloatDetailsMapping>(); }
	};
}
