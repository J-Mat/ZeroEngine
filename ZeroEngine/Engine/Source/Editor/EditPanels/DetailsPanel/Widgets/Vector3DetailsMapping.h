#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FVector3DDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FVector3DDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property) override;
	private:
		float m_ColumnWidth = 100.0f;
	};
}