#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FFloatSliderDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FFloatSliderDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}
