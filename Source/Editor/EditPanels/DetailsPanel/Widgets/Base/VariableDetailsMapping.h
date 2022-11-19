#pragma once
#include "DetailsMapping.h"
#include "World/Base/VariateProperty.h"

namespace Zero
{
	class FVariableDetailsMapping : public FDetailsMapping
	{
	public:
		bool UpdateDetailsWidget(UProperty* InProperty, bool bDisplayTag = true);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return nullptr; }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* PropertyTag) = 0;
	};
}