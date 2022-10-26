#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FRotation3DetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FRotation3DetailsMapping>(); }
	private:
		float m_ColumnWidth = 100.0f;
	};
}