#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FTextureHandleDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FTextureHandleDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}