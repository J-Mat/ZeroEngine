#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FColor3DetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FColor3DetailsMapping>(); }
	private:
		float m_ColumnWidth = 100.0f;
	};
}