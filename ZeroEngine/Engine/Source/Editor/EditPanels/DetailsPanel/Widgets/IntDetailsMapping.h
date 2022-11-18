#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FIntDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FIntDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}
