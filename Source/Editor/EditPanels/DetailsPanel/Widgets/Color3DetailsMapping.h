#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FColor3DetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FColor3DetailsMapping>(); }

	private:
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	private:
		float m_ColumnWidth = 100.0f;
	};
}