#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FTextureDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FTextureDetailsMapping>(); }
	};
}