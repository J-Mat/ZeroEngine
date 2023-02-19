#pragma once

#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FArrayObjectDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FArrayObjectDetailsMapping>(); }

	private:
		void UpdateWidget(UProperty* Property);
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}