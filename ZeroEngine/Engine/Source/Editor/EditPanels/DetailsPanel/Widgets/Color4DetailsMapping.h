#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FColor4DetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* Property);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FColor4DetailsMapping>(); }
	private:
		float m_ColumnWidth = 100.0f;
	};
}