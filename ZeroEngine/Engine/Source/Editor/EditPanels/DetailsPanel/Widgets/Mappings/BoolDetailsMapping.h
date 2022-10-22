#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FBoolDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FVariableDetailsMapping>(); }
	};
}
