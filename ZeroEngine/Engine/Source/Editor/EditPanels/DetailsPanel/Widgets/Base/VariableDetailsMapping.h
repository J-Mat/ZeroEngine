#pragma once
#include "DetailsMapping.h"
#include "World/Base/VariateProperty.h"

namespace Zero
{
	class FVariableDetailsMapping : public FDetailsMapping
	{
	public:
		bool UpdateDetailsWidget(UProperty* InProperty);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return nullptr; }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* InProperty) = 0;
	};
}