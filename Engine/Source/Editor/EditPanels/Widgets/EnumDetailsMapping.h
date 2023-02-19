#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FEnumDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FEnumDetailsMapping>(); }
	private:
		int GetEnumIndex(const FEnumElement& EnumElement, int EnumValue);
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}
