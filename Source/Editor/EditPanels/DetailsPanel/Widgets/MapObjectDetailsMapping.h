#pragma once

#include "Base/VariableDetailsMapping.h"
#include "World/Base/MapPropretyObject.h"

namespace Zero
{
	class FMapObjectDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FMapObjectDetailsMapping>(); }
	private:
		void UpdateWidget(UProperty* KeyProperty);
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}