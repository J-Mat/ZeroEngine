#pragma once

#include "Base/VariableDetailsMapping.h"
#include "World/Object/MapPropretyObject.h"

namespace Zero
{
	class FMapObjectDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FMapObjectDetailsMapping>(); }
	private:
		void UpdateWidget(UMapProperty* MapProperty, UProperty* KeyProperty);
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
	};
}