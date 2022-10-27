#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FVector4DDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FVector4DDetailsMapping>(); }
	};
}