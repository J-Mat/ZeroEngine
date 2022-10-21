#pragma once

#include "Base/PropertyDetailsMapping.h"

namespace Zero
{
	class FStringDetailsMapping : public FPropertyDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FPropertyDetailsMapping> MakeDetailMapping() { return CreateRef<FStringDetailsMapping>(); }
	};
}