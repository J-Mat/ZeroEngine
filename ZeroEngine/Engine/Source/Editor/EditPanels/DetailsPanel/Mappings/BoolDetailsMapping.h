#pragma once

#include "../Base/PropertyDetailsMapping.h"

namespace Zero
{
	class FBoolDetailsMapping : public FPropertyDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty);
		static Ref<FPropertyDetailsMapping> MakeDetailMapping() { return CreateRef<FPropertyDetailsMapping>(); }
	};
}
