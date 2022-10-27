#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FVector3DDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FVector3DDetailsMapping>(); }
	private:
		float m_ColumnWidth = 100.0f;
	};
}