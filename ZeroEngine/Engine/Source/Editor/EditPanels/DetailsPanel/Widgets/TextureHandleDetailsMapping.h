#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FTextureHandleDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FTextureHandleDetailsMapping>(); }
	};
}