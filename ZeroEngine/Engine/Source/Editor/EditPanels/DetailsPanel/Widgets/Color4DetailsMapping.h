#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FColor4DetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FColor4DetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property) override;
	private:
		float m_ColumnWidth = 100.0f;
	};
}