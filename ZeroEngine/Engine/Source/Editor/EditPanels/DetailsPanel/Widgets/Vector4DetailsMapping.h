#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FVector4DDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FVector4DDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property) override;
	};
}