#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FStringDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FStringDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}