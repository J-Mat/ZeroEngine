#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FFloatDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FFloatDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* InProperty) override;
	};
}
