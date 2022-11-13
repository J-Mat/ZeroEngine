#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FRotation3DetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FRotation3DetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property) override;
	private:
		float m_ColumnWidth = 100.0f;
	};
}