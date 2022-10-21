#pragma once
#include "DetailsMapping.h"
#include "World/Base/PropertyObject.h"

namespace Zero
{
	class FClassDetailsMapping : public FDetailsMapping
	{
	public:
		virtual void UpdateDetailsWidget(UCoreObject* CoreObject) { return false; }
		static Ref<FClassDetailsMapping> MakeDetailMapping() { return nullptr; }
	};
}