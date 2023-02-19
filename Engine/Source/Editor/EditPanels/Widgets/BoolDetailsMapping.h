#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FBoolDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FBoolDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}
