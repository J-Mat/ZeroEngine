#pragma once
#include "DetailsMapping.h"
#include "World/Base/PropertyObject.h"

namespace Zero
{
	class FClassDetailsMapping : public FDetailsMapping
	{
	public:
		virtual void UpdateDetailsWidget(UCoreObject* CoreObject);
		static Ref<FClassDetailsMapping> MakeDetailMapping();
	};
}