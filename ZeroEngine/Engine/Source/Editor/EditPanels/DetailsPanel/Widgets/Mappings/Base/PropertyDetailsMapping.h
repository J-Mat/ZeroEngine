#pragma once
#include "DetailsMapping.h"
#include "World/Base/PropertyObject.h"

namespace Zero
{
	class FPropertyDetailsMapping : public FDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty) { return false; }
		static Ref<FPropertyDetailsMapping> MakeDetailMapping() { return nullptr; }
	};
}