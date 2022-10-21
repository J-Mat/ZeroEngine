#pragma once

#include "Base/PropertyDetailsMapping.h"

namespace Zero
{
	class FVector4DDetailsMapping : public FPropertyDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FPropertyDetailsMapping> MakeDetailMapping() { return CreateRef<FVector4DDetailsMapping>(); }
	};
}